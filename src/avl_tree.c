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

/*
Portions of this code were copied from the following work.

Copyright © 2020 Parewa Labs Pvt. Ltd.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.
*/


/* Implementation of a simple generic AVL Binary Search Tree */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl_tree.h"

#ifndef MAX
  #define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif

struct node_t {
    void *data;
    struct node_t *left;
    struct node_t *right;
    int height;
};

struct tree_t {
    int (*compare_function)(void *left, void *right);
    struct node_t *root;
};

// Calculate height
static int
_height(struct node_t *node) {
  if (node == NULL)
    return 0;
  return node->height;
}


// Create a node

static struct node_t *
_new_node(void *data) {
    struct node_t *node = (struct node_t *)malloc(sizeof(struct node_t));
    node->data = data;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    return (node);
}

// Right rotate
struct node_t *_right_rotate(struct node_t *y) {
    struct node_t *x = y->left;
    struct node_t *T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = MAX(_height(y->left), _height(y->right)) + 1;
    x->height = MAX(_height(x->left), _height(x->right)) + 1;

    return x;
}

// Left rotate
struct node_t *_left_rotate(struct node_t *x) {
    struct node_t *y = x->right;
    struct node_t *T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = MAX(_height(x->left), _height(x->right)) + 1;
    y->height = MAX(_height(y->left), _height(y->right)) + 1;

    return y;
}

// Get the balance factor
static int
_get_balance(struct node_t *node) {
    if (node == NULL) return 0;
    return _height(node->left) - _height(node->right);
}

// Insert node
static struct node_t *
_insert_node(struct tree_t *tree, struct node_t *node, void *key) {
    // Find the correct position to insertNode the node and insertNode it
    if (node == NULL) {
        return _new_node(key);
    }

    if(tree->compare_function(key, node->data) < 0) {
        node->left = _insert_node(tree, node->left, key);
    } else if(tree->compare_function(key, node->data) > 0) {
        node->right = _insert_node(tree, node->right, key);
    } else {
        return node;
    }

    // Update the balance factor of each node and
    // Balance the tree
    node->height = 1 + MAX(_height(node->left), _height(node->right));

    int balance = _get_balance(node);
    if(balance > 1 && tree->compare_function(key, node->left->data) < 0) {
        return _right_rotate(node);
    }

    if(balance < -1 && tree->compare_function(key, node->right->data) > 0) {
        return _left_rotate(node);
    }

    if(balance > 1 && tree->compare_function(key, node->left->data) > 0) {
        node->left = _left_rotate(node->left);
        return _right_rotate(node);
    }

    if(balance < -1 && tree->compare_function(key, node->right->data) < 0) {
        node->right = _right_rotate(node->right);
        return _left_rotate(node);
    }

    return node;
}


static struct node_t *
_smallest_node(struct node_t *node) {
    struct node_t *current = node;

    while (current->left != NULL) {
        current = current->left;
    }

    return current;
}

/* Delete a node.  Frees the node structure but not the user supplied pointer */
static struct node_t *
_delete_node(struct tree_t *tree, struct node_t *root, void *key) {
    // Find the node and delete it
    if (root == NULL)
        return root;

    if (tree->compare_function(key, root->data)<0)
        root->left = _delete_node(tree, root->left, key);

    else if (tree->compare_function(key, root->data) > 0)
        root->right = _delete_node(tree, root->right, key);

    else {
      if ((root->left == NULL) || (root->right == NULL)) {
        struct node_t *temp = root->left ? root->left : root->right;

        if (temp == NULL) {
          temp = root;
          root = NULL;
        } else
          *root = *temp;
        free(temp);
      } else {
        struct node_t *temp = _smallest_node(root->right);

        root->data = temp->data;

        root->right = _delete_node(tree, root->right, temp->data);
      }
    }

    if (root == NULL) {
        return root;
    }

    // Update the balance factor of each node and
    // balance the tree
    root->height = 1 + MAX(_height(root->left), _height(root->right));

    int balance = _get_balance(root);
    if(balance > 1 && _get_balance(root->left) >= 0) {
        return _right_rotate(root);
    }

    if(balance > 1 && _get_balance(root->left) < 0) {
        root->left = _left_rotate(root->left);
        return _right_rotate(root);
    }

    if(balance < -1 && _get_balance(root->right) <= 0) {
        return _left_rotate(root);
    }

    if(balance < -1 && _get_balance(root->right) > 0) {
        root->right = _right_rotate(root->right);
        return _left_rotate(root);
    }

    return root;
}


static void
_inorder(node_t *root, void (*f)(void *, void *), void *udata) {
    if (root != NULL) {
        _inorder(root->left, f, udata);
        f(root->data, udata);
        _inorder(root->right, f, udata);
    }
}


/* Public Facing Functions */
/* Create a new AVL Tree.  The function given should take
 * two pointers and be able to compare them.  It should
 * return values similar to strcmp() */
struct tree_t *
avl_new_tree(int (*f)(void *left, void *right)) {
    struct tree_t *new_tree = malloc(sizeof(struct tree_t));
    new_tree->compare_function = f;
    new_tree->root = NULL;
    return new_tree;
}


/* Insert a node with data.  The data pointer is never manipulated
 * by this library.  It is the responsibilty of the caller to
 * allocate and free whatever this pointer points to. */
int
avl_insert_node(tree_t *tree, void *data) {
    tree->root = _insert_node(tree, tree->root, data);
    return 0;
}


void
avl_delete_node(tree_t *tree, void *data) {
    tree->root = _delete_node(tree, tree->root, data);
}


/* Traverses the tree looking for a node that matches 'key'
 * based on the compare function that is set in the tree */
void *
avl_search(tree_t *tree, void *key) {
    node_t *root = tree->root;
    int result;

    while(root != NULL) {
        result = tree->compare_function(root->data, key);
        if(result  == 0) {
            return root->data;
        }
        if(result > 0) {
            root = root->right;
        } else {
            root = root->left;
        }
    }
    return NULL;
}


/* Traverses the tree in order calling 'f' for each node.  The first
 * pointer to f() will be the node data and the second the udata
 * pointer passed to this function */
void
avl_inorder(tree_t *tree, void (*f)(void *, void *), void *udata) {
    _inorder(tree->root, f, udata);
}

