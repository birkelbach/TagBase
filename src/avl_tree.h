/*  TagBase - An open source Tag Data Storage Library
 *  Copyright (c) 2026 Phil Birkelbach
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

typedef struct tree_t avl_tree_t;

avl_tree_t *avl_new_tree(int (*f)(void *left, void *right));
void avl_set_free_function(struct tree_t *tree, void (*f)(void *node_data));
void avl_del_tree(avl_tree_t *tree);
int avl_insert_node(avl_tree_t *tree, void *data);
void avl_delete_node(avl_tree_t *tree, void *data);
void *avl_search(avl_tree_t *tree, void *key);
void avl_inorder(avl_tree_t *tree, void (*f)(void *, void *), void *udata);
