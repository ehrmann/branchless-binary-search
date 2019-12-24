#include "gtest/gtest.h"
#include "bbsearch.h"

#include <cstdio>
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <random>
#include <set>
#include <chrono>

static const int TOTAL_TRIALS = 20000;

static int compare_int32(const void * a, const void * b) {
    if (*(int32_t *) a < *(int32_t *) b) {
        return -1;
    } else if (*(int32_t *) a >  *(int32_t *) b) {
        return 1;
    } else {
        return 0;
    }
}

TEST(BranchlessBinarySearchTest, RandomSparse) {
    std::default_random_engine generator;
    std::vector<int32_t> array;
    for (int trial = 0; trial < TOTAL_TRIALS; ++trial) {
        array.clear();

        for (auto i = std::uniform_int_distribution<int>(0, 1000)(generator); i >= 0; --i) {
            array.push_back(generator());
        }

        int32_t key = array[0];
        std::sort(array.begin(), array.end());

        ptrdiff_t index = bbsearch(array.data(), array.size(), key);
        ASSERT_EQ(key, array[index]);
    }
}

TEST(BranchlessBinarySearchTest, RandomDense) {
    std::default_random_engine generator;
    std::vector<int32_t> array;

    for (int trial = 0; trial < TOTAL_TRIALS; ++trial) {
        array.clear();

        for (auto i = std::uniform_int_distribution<int>(0, 1000)(generator); i >= 0; --i) {
            array.push_back(generator() % 100);
        }

        int32_t key = array[0];
        std::sort(array.begin(), array.end());

        ptrdiff_t index = bbsearch(array.data(), array.size(), key);
        ASSERT_EQ(key, array[index]);
    }
}

TEST(BranchlessBinarySearchTest, Empty) {
    std::vector<int32_t> array;
    ptrdiff_t index = bbsearch(array.data(), array.size(), 42);
    ASSERT_EQ(-1, index);
}

TEST(BranchlessBinarySearchTest, MissingSparse) {
    std::default_random_engine generator;
    std::vector<int32_t> array;
    for (int trial = 0; trial < TOTAL_TRIALS; ++trial) {
        array.clear();
        std::set<int32_t> set;

        for (auto i = std::uniform_int_distribution<int>(0, 1000)(generator); i >= 0; --i) {
            auto val = generator();
            set.insert(val);
            array.push_back(val);
        }

        std::sort(array.begin(), array.end());

        int32_t key;
        do {
            key = generator();
        } while (set.find(key) != set.end());

        ptrdiff_t index = bbsearch(array.data(), array.size(), key);

        ASSERT_LT(index, 0);
        ptrdiff_t insertion_index = -(index + 1);
        if (insertion_index > 0) {
            ASSERT_GE(key, array[insertion_index - 1]);
        }
        if (insertion_index < array.size()) {
            ASSERT_LE(key,array[insertion_index]);
        }
    }
}

TEST(BranchlessBinarySearchTest, MissingDense) {
    std::default_random_engine generator;
    std::vector<int32_t> array;
    for (int trial = 0; trial < TOTAL_TRIALS; ++trial) {
        array.clear();
        int32_t key = generator() % 100;

        for (auto i = std::uniform_int_distribution<int>(0, 1000)(generator); i >= 0; --i) {
            int32_t val;
            while ((val = generator()) == key);
            array.push_back(val);
        }

        std::sort(array.begin(), array.end());

        ptrdiff_t index = bbsearch(array.data(), array.size(), key);

        ASSERT_LT(index, 0);
        ptrdiff_t insertion_index = -(index + 1);
        if (insertion_index > 0) {
            ASSERT_GE(key, array[insertion_index - 1]);
        }
        if (insertion_index < array.size()) {
            ASSERT_LE(key,array[insertion_index]);
        }
    }
}

TEST(BranchlessBinarySearchTest, BenchmarkSparse) {
    std::default_random_engine generator;
    std::vector<int32_t> array;

    for (int i = 0; i < 65536; ++i) {
        array.push_back(i);
    }

    int64_t branchless_nanos = 0;
    int64_t branchless_trials = 0;
    int64_t builtin_nanos = 0;
    int64_t builtin_trials = 0;

    int use_branchless = 0;

    for (int trial = 0; trial < 1000000; ++trial) {
        int32_t key = std::uniform_int_distribution<int>(0, array.size() - 1)(generator);

        auto begin = std::chrono::high_resolution_clock::now();
        if (use_branchless) {
            bbsearch(array.data(), array.size(), key);
        } else {
            bsearch(&key, array.data(), array.size(), sizeof(array[0]), compare_int32);
        }
        auto end = std::chrono::high_resolution_clock::now();

        if (use_branchless) {
            branchless_nanos += std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count();
            ++branchless_trials;
        } else {
            builtin_nanos += std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count();
            ++builtin_trials;
        }
        use_branchless = !use_branchless;
    }

    printf("mean branchless time: %ldns; mean builtin time: %ldns\n",
            branchless_nanos / branchless_trials,
            builtin_nanos / builtin_trials);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
