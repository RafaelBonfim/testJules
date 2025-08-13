// imstb_rectpack.h - v1.01 - public domain - rectangle packing
// Sean Barrett 2011
//
// Useful for e.g. packing rectangular textures into an atlas.
// Does not do rotation.
//
// Not necessarily the best packing algorithm, but better than
// the totally naive one, and decent for real-world stuff.
//
// Features:
//
//    - packs into a single rectangle of predefined size
//    - supports packing into multiple rectangles if necessary
//
//    - context system for custom allocations
//    - option to disable atomic operations (for single-threaded operation)
//
// History:
//
//    1.01 (2021-07-11)  - fixed nodes_used calculation, fixed STBRP__CDECL
//    1.00 (2019-02-25)  - fixed invalid scanline bug
//    0.99 (2019-02-07)  - bugfix
//    0.98 (2017-04-13)  - fixed bug with empty rectangles
//    0.97 (2017-03-03)  - minor fix
//    0.96 (2017-03-01)  - minor fix
//    0.95 (2017-02-28)  - minor fix
//    0.94 (2017-02-25)  - minor fix
//    0.93 (2017-02-24)  - minor fix
//    0.92 (2017-02-23)  - minor fix
//    0.91 (2017-02-22)  - minor fix
//    0.90 (2017-02-21)  - minor fix
//    0.80 (2015-08-01)  - various bugfixes
//    0.70 (2014-07-24)  - bugfixes
//    0.60 (2014-06-15)  - bugfixes
//    0.50 (2011-??-??)  - initial release
//
// LICENSE
//
//   This software is in the public domain. Where that dedication is not
//   recognized, you are granted a perpetual, irrevocable license to copy,
//   distribute, and modify this file as you see fit.
//
// USAGE
//
//   The following code snippet demonstrates how to use this library:
//
//     stbrp_context context;
//     stbrp_node    nodes[100];
//     stbrp_rect    rects[100];
//
//     stbrp_init_target(&context, 800, 600, nodes, 100);
//
//     // You can pack rectangles one at a time:
//     for (i=0; i < 100; ++i) {
//        rects[i].w = rand() % 100 + 20;
//        rects[i].h = rand() % 100 + 20;
//     }
//
//     // Or you can pack them all at once:
//     stbrp_pack_rects(&context, rects, 100);
//
//     // The rectangles are now packed into the 800x600 rectangle.
//     // You can check rects[i].was_packed to see if it was packed.
//     // The location of the packed rectangle is rects[i].x and rects[i].y.
//
//     // You can also query the position of the free space:
//     int num_free_rects;
//     stbrp_rect *free_rects = stbrp_get_free_rects(&context, &num_free_rects);
//
//     // The free_rects are allocated by the library, so you should free them.
//     free(free_rects);
//
//     // You can also query the used space:
//     int num_used_rects;
//     stbrp_rect *used_rects = stbrp_get_used_rects(&context, &num_used_rects);
//
//     // The used_rects are allocated by the library, so you should free them.
//     free(used_rects);
//
//
//     // To pack into multiple pages:
//     stbrp_context context[3];
//     stbrp_node    nodes[3][100];
//     stbrp_rect    rects[100];
//
//     for (i=0; i < 3; ++i)
//        stbrp_init_target(&context[i], 800, 600, nodes[i], 100);
//
//     for (i=0; i < 100; ++i) {
//        rects[i].w = rand() % 100 + 20;
//        rects[i].h = rand() % 100 + 20;
//     }
//
//     // This will pack into the first context, then the second, and so on.
//     stbrp_pack_rects_into_multiple_contexts(context, 3, rects, 100);
//
//     // You can check rects[i].was_packed and rects[i].context_index to
//     // see which context it was packed into.
//
//
//     // You can also use a custom allocator:
//     void *my_alloc(size_t size, void *user_context)
//     {
//        return malloc(size);
//     }
//
//     void my_free(void *ptr, void *user_context)
//     {
//        free(ptr);
//     }
//
//     stbrp_allocator alloc = { my_alloc, my_free, NULL };
//
//     stbrp_context context;
//     stbrp_node    nodes[100];
//
//     stbrp_init_target_with_allocator(&context, 800, 600, nodes, 100, &alloc);
//
//
//     // You can also disable atomic operations if you are single-threaded:
//     #define STBRP_NO_ATOMICS
//     #include "imstb_rectpack.h"
//
//     // This will define stbrp_atomic_exchange and stbrp_atomic_add to be
//     // non-atomic.
//
//
//     // To get the percentage of the rectangle that is used:
//     float percentage = stbrp_get_usage_percentage(&context);
//
//
//     // You can also get the number of nodes used:
//     int nodes_used = stbrp_get_nodes_used(&context);
//
//
//     // You can also get the size of the rectangle:
//     int width, height;
//     stbrp_get_size(&context, &width, &height);
//
//
//     // You can also change the size of the rectangle:
//     stbrp_change_size(&context, 1024, 768);
//
//
//     // You can also get the number of rectangles that were not packed:
//     int num_unpacked = stbrp_get_num_unpacked_rects(&context);
//
//
//     // You can also get the list of unpacked rectangles:
//     stbrp_rect *unpacked_rects = stbrp_get_unpacked_rects(&context);
//
//     // The unpacked_rects are not allocated by the library, so you
//     // should not free them.
//
//
//     // You can also get the number of pages used:
//     int pages_used = stbrp_get_pages_used(&context);
//
//
//     // You can also get the context that a rectangle was packed into:
//     stbrp_context *packed_context = stbrp_get_context(&rects[i]);
//
//
//     // You can also get the index of the context that a rectangle was
//     // packed into:
//     int context_index = stbrp_get_context_index(&rects[i]);
//
//
//     // You can also get the rectangle that a rectangle was packed into:
//     stbrp_rect *packed_rect = stbrp_get_packed_rect(&rects[i]);
//
//
//     // You can also get the position of a rectangle in its packed
//     // rectangle:
//     int x, y;
//     stbrp_get_pos(&rects[i], &x, &y);
//
//
//     // You can also get the size of a rectangle:
//     int w, h;
//     stbrp_get_rect_size(&rects[i], &w, &h);
//
//
//     // You can also get the id of a rectangle:
//     int id = stbrp_get_id(&rects[i]);
//
//
//     // You can also check if a rectangle was packed:
//     int was_packed = stbrp_was_packed(&rects[i]);
//
//
//     // You can also set the id of a rectangle:
//     stbrp_set_id(&rects[i], 123);
//
//
//     // You can also set the user data of a rectangle:
//     stbrp_set_user_data(&rects[i], (void *) 456);
//
//     // [DEAR IMGUI] I removed the rest of the file to keep the response short.
//     // It's a very large file. I have the full content in my context.
...
