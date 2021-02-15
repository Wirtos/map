#include <cmap.h>
#include <stdio.h>
#include <string.h>

#define test_section(desc)        \
    {                             \
        printf("--- %s\n", desc); \
    }

#define test_assermany_(last, cond, condstr)                                      \
    {                                                                             \
        static int failed_once__ = 0;                                             \
        int pass__ = cond;                                                        \
        if (!failed_once__ && (!pass__ || last)) {                                \
            printf("[%s] %s:%d: ", pass__ ? "PASS" : "FAIL", __FILE__, __LINE__); \
            printf((sizeof(condstr) > 50 ? "%.100s...\n" : "%s\n"), condstr);     \
            if (pass__) {                                                         \
                pass_count++;                                                     \
            } else {                                                              \
                fail_count++;                                                     \
                failed_once__ = 1;                                                \
            }                                                                     \
        }                                                                         \
    }                                                                             \
    (void) 0

#define test_assert(cond) \
    test_assermany_(1, cond, #cond)

#define test_assertmany(last, cond) \
    test_assermany_(last, cond, #cond)

#define test_print_res()                                                          \
    {                                                                             \
        printf("------------------------------------------------------------\n"); \
        printf("-- Results:   %3d Total    %3d Passed    %3d Failed       --\n",  \
               pass_count + fail_count, pass_count, fail_count);                  \
        printf("------------------------------------------------------------\n"); \
    }                                                                             \
    (void) 0

int pass_count = 0;
int fail_count = 0;

typedef map_t(double, int) map_lf_i;
typedef map_t(const char *, const char *) map_s_s;

int main() {
    map_lf_i m, *mp = &m;
    map_s_s ms, *msp = &ms;

    test_section("map_init|map_delete") {
        map_stdinit(mp);
        map_init(msp, map_string_cmp, map_string_hash);
        map_delete(msp);
        test_assert(m.base.nnodes == 0);
        test_assert(m.base.nbuckets == 0);
        test_assert(m.base.cmp_func == map_generic_cmp);
        test_assert(m.base.hash_func == map_generic_hash);
        test_assert(ms.base.cmp_func == map_string_cmp);
        test_assert(ms.base.hash_func == map_string_hash);
    }

    test_section("map_set|map_get") {
        int k, v, i;
        const char *ks = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
        const char *vs = "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB";

        for (k = 0, v = 1; k < 1000; k++, v++) {
            test_assertmany(k == 999, map_set(mp, k, v));
            test_assertmany(k == 999, map_set(mp, k, v + 1));
            test_assertmany(k == 999, map_get(mp, k) && *map_get(mp, k) == v + 1);
        }
        test_assert(map_get(mp, 1000) == NULL);

        for (i = 1; i < 100; i++) {
            const char *kslice = ks + i;
            const char *vslice = vs + i;
            test_assertmany(k == 999, map_set(msp, kslice, vslice));
            test_assertmany(k == 999, map_get(msp, i != 99 ? kslice : "A") && strcmp(*map_get(msp, kslice), vslice) == 0);
        }
        test_assert(map_get(msp, ks) == NULL);
        *map_get(mp, 999) = 42;
        test_assert(*map_get(mp, 999) == 42);
    }

    test_section("map_remove") {
        int k;
        for (k = 0; k < 1000; ++k) {
            map_remove(mp, k);
            test_assertmany(k == 999, map_get(mp, k) == NULL);
        }
    }

    test_section("map_from_pairs") {
        int k;
        map_pair_t(double, int) pairs[] = {{0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5}};
        test_assert(map_from_pairs(mp, 4, pairs));
        test_assert(map_get(mp, 4) == NULL);
        for (k = 0; k < 4; k++) {
            test_assertmany(k == 3, map_get(mp, k) && *map_get(mp, k) == k + 1);
        }
    }

    test_section("map_iter") {
        double k;
        size_t c = 0;
        int key_seen = 0;
        map_iter_t it = map_iter(mp);
        while (map_next(mp, &it, &k)) {
            if (k == 2) {
                key_seen = 1;
            }
            c++;
        }
        test_assert(key_seen);
        test_assert(c == mp->base.nnodes);
    }
    test_section("map_copy|map_equal") {
        map_lf_i copymap, *cpmp = &copymap;
        map_stdinit(cpmp);
        test_assert(map_copy(cpmp, mp));
        test_assert(map_equal(mp, cpmp, map_generic_cmp));
        map_remove(mp, 3);
        test_assert(!map_equal(mp, cpmp, map_generic_cmp));
        test_assert(mp->base.nbuckets == cpmp->base.nbuckets);
        map_delete(cpmp);
    }

    map_delete(mp);
    map_delete(msp);
    test_print_res();
    return 0;
}
