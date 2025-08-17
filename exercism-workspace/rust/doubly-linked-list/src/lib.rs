// this module adds some functionality based on the required implementations
// here like: `LinkedList::pop_back` or `Clone for LinkedList<T>`
// You are free to use anything in it, but it's mainly for the test framework.
mod pre_implemented;

use std::ptr::NonNull;
use std::marker::PhantomData;

struct Node<T> {
    value: T,
    next: Option<NonNull<Node<T>>>,
    prev: Option<NonNull<Node<T>>>,
}

pub struct LinkedList<T> {
    head: Option<NonNull<Node<T>>>,
    tail: Option<NonNull<Node<T>>>,
    len: usize,
    _phantom: PhantomData<T>,
}

pub struct Cursor<'a, T> {
    list: &'a mut LinkedList<T>,
    current: Option<NonNull<Node<T>>>,
}

pub struct Iter<'a, T> {
    head: Option<NonNull<Node<T>>>,
    _phantom: PhantomData<&'a T>,
}

impl<T> LinkedList<T> {
    pub fn new() -> Self {
        LinkedList {
            head: None,
            tail: None,
            len: 0,
            _phantom: PhantomData,
        }
    }

    // You may be wondering why it's necessary to have is_empty()
    // when it can easily be determined from len().
    // It's good custom to have both because len() can be expensive for some types,
    // whereas is_empty() is almost always cheap.
    // (Also ask yourself whether len() is expensive for LinkedList)
    pub fn is_empty(&self) -> bool {
        self.len == 0
    }

    pub fn len(&self) -> usize {
        self.len
    }

    /// Return a cursor positioned on the front element
    pub fn cursor_front(&mut self) -> Cursor<'_, T> {
        Cursor {
            current: self.head,
            list: self,
        }
    }

    /// Return a cursor positioned on the back element
    pub fn cursor_back(&mut self) -> Cursor<'_, T> {
        Cursor {
            current: self.tail,
            list: self,
        }
    }

    /// Return an iterator that moves from front to back
    pub fn iter(&self) -> Iter<'_, T> {
        Iter {
            head: self.head,
            _phantom: PhantomData,
        }
    }
}

impl<T> Drop for LinkedList<T> {
    fn drop(&mut self) {
        while self.pop_front().is_some() {}
    }
}

// the cursor is expected to act as if it is at the position of an element
// and it also has to work with and be able to insert into an empty list.
impl<'a, T> Cursor<'a, T> {
    /// Take a mutable reference to the current element
    pub fn peek_mut(&mut self) -> Option<&mut T> {
        self.current.map(|node| unsafe {
            &mut (*node.as_ptr()).value
        })
    }

    /// Move one position forward (towards the back) and
    /// return a reference to the new position
    #[allow(clippy::should_implement_trait)]
    pub fn next(&mut self) -> Option<&mut T> {
        if let Some(current) = self.current {
            unsafe {
                self.current = (*current.as_ptr()).next;
            }
        }
        self.peek_mut()
    }

    /// Move one position backward (towards the front) and
    /// return a reference to the new position
    pub fn prev(&mut self) -> Option<&mut T> {
        if let Some(current) = self.current {
            unsafe {
                self.current = (*current.as_ptr()).prev;
            }
        }
        self.peek_mut()
    }

    /// Remove and return the element at the current position and move the cursor
    /// to the neighboring element that's closest to the back. This can be
    /// either the next or previous position.
    pub fn take(&mut self) -> Option<T> {
        self.current.map(|node| unsafe {
            let node_ptr = node.as_ptr();
            let node_box = Box::from_raw(node_ptr);
            
            // Update links
            if let Some(prev) = node_box.prev {
                (*prev.as_ptr()).next = node_box.next;
            } else {
                self.list.head = node_box.next;
            }
            
            if let Some(next) = node_box.next {
                (*next.as_ptr()).prev = node_box.prev;
            } else {
                self.list.tail = node_box.prev;
            }
            
            // Move cursor
            self.current = node_box.next.or(node_box.prev);
            
            // Update length
            self.list.len -= 1;
            
            node_box.value
        })
    }

    pub fn insert_after(&mut self, element: T) {
        let new_node = Box::new(Node {
            value: element,
            next: None,
            prev: None,
        });
        let new_node_ptr = NonNull::new(Box::into_raw(new_node)).unwrap();
        
        if let Some(current) = self.current {
            unsafe {
                let current_node = current.as_ptr();
                let old_next = (*current_node).next;
                
                // Set new node's links
                (*new_node_ptr.as_ptr()).prev = Some(current);
                (*new_node_ptr.as_ptr()).next = old_next;
                
                // Update current's next
                (*current_node).next = Some(new_node_ptr);
                
                // Update old next's prev (if it exists)
                if let Some(old_next_node) = old_next {
                    (*old_next_node.as_ptr()).prev = Some(new_node_ptr);
                } else {
                    // We inserted at the tail
                    self.list.tail = Some(new_node_ptr);
                }
            }
        } else {
            // List is empty, insert as the only element
            self.list.head = Some(new_node_ptr);
            self.list.tail = Some(new_node_ptr);
            self.current = Some(new_node_ptr);
        }
        
        self.list.len += 1;
    }

    pub fn insert_before(&mut self, element: T) {
        let new_node = Box::new(Node {
            value: element,
            next: None,
            prev: None,
        });
        let new_node_ptr = NonNull::new(Box::into_raw(new_node)).unwrap();
        
        if let Some(current) = self.current {
            unsafe {
                let current_node = current.as_ptr();
                let old_prev = (*current_node).prev;
                
                // Set new node's links
                (*new_node_ptr.as_ptr()).next = Some(current);
                (*new_node_ptr.as_ptr()).prev = old_prev;
                
                // Update current's prev
                (*current_node).prev = Some(new_node_ptr);
                
                // Update old prev's next (if it exists)
                if let Some(old_prev_node) = old_prev {
                    (*old_prev_node.as_ptr()).next = Some(new_node_ptr);
                } else {
                    // We inserted at the head
                    self.list.head = Some(new_node_ptr);
                }
            }
        } else {
            // List is empty, insert as the only element
            self.list.head = Some(new_node_ptr);
            self.list.tail = Some(new_node_ptr);
            self.current = Some(new_node_ptr);
        }
        
        self.list.len += 1;
    }
}

impl<'a, T> Iterator for Iter<'a, T> {
    type Item = &'a T;

    fn next(&mut self) -> Option<&'a T> {
        self.head.map(|node| unsafe {
            let node_ref = &*node.as_ptr();
            self.head = node_ref.next;
            &node_ref.value
        })
    }
}

