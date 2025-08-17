/// Yields each item of a and then each item of b
pub fn append<I, J>(a: I, b: J) -> impl Iterator<Item = I::Item>
where
    I: Iterator,
    J: Iterator<Item = I::Item>,
{
    AppendIterator::new(a, b)
}

struct AppendIterator<I, J> {
    first: Option<I>,
    second: Option<J>,
}

impl<I, J> AppendIterator<I, J> {
    fn new(first: I, second: J) -> Self {
        Self {
            first: Some(first),
            second: Some(second),
        }
    }
}

impl<I, J> Iterator for AppendIterator<I, J>
where
    I: Iterator,
    J: Iterator<Item = I::Item>,
{
    type Item = I::Item;

    fn next(&mut self) -> Option<Self::Item> {
        if let Some(ref mut first) = self.first {
            if let Some(item) = first.next() {
                return Some(item);
            } else {
                self.first = None;
            }
        }
        if let Some(ref mut second) = self.second {
            second.next()
        } else {
            None
        }
    }
}

/// Combines all items in all nested iterators inside into one flattened iterator
pub fn concat<I>(nested_iter: I) -> impl Iterator<Item = <I::Item as Iterator>::Item>
where
    I: Iterator,
    I::Item: Iterator,
{
    ConcatIterator::new(nested_iter)
}

struct ConcatIterator<I> where I: Iterator, I::Item: Iterator {
    outer: I,
    current: Option<I::Item>,
}

impl<I> ConcatIterator<I> where I: Iterator, I::Item: Iterator {
    fn new(outer: I) -> Self {
        Self {
            outer,
            current: None,
        }
    }
}

impl<I> Iterator for ConcatIterator<I>
where
    I: Iterator,
    I::Item: Iterator,
{
    type Item = <I::Item as Iterator>::Item;

    fn next(&mut self) -> Option<Self::Item> {
        loop {
            if let Some(ref mut current) = self.current {
                if let Some(item) = current.next() {
                    return Some(item);
                }
            }
            self.current = self.outer.next();
            if self.current.is_none() {
                return None;
            }
        }
    }
}

/// Returns an iterator of all items in iter for which `predicate(item)` is true
pub fn filter<I, F>(iter: I, predicate: F) -> impl Iterator<Item = I::Item>
where
    I: Iterator,
    F: Fn(&I::Item) -> bool,
{
    FilterIterator::new(iter, predicate)
}

struct FilterIterator<I, F> {
    iter: I,
    predicate: F,
}

impl<I, F> FilterIterator<I, F> {
    fn new(iter: I, predicate: F) -> Self {
        Self { iter, predicate }
    }
}

impl<I, F> Iterator for FilterIterator<I, F>
where
    I: Iterator,
    F: Fn(&I::Item) -> bool,
{
    type Item = I::Item;

    fn next(&mut self) -> Option<Self::Item> {
        while let Some(item) = self.iter.next() {
            if (self.predicate)(&item) {
                return Some(item);
            }
        }
        None
    }
}

pub fn length<I: Iterator>(mut iter: I) -> usize {
    let mut count = 0;
    while iter.next().is_some() {
        count += 1;
    }
    count
}

/// Returns an iterator of the results of applying `function(item)` on all iter items
pub fn map<I, F, U>(iter: I, function: F) -> impl Iterator<Item = U>
where
    I: Iterator,
    F: Fn(I::Item) -> U,
{
    MapIterator::new(iter, function)
}

struct MapIterator<I, F> {
    iter: I,
    function: F,
}

impl<I, F> MapIterator<I, F> {
    fn new(iter: I, function: F) -> Self {
        Self { iter, function }
    }
}

impl<I, F, U> Iterator for MapIterator<I, F>
where
    I: Iterator,
    F: Fn(I::Item) -> U,
{
    type Item = U;

    fn next(&mut self) -> Option<Self::Item> {
        self.iter.next().map(&self.function)
    }
}

pub fn foldl<I, F, U>(mut iter: I, initial: U, function: F) -> U
where
    I: Iterator,
    F: Fn(U, I::Item) -> U,
{
    let mut accumulator = initial;
    while let Some(item) = iter.next() {
        accumulator = function(accumulator, item);
    }
    accumulator
}

pub fn foldr<I, F, U>(mut iter: I, initial: U, function: F) -> U
where
    I: DoubleEndedIterator,
    F: Fn(U, I::Item) -> U,
{
    let mut accumulator = initial;
    while let Some(item) = iter.next_back() {
        accumulator = function(accumulator, item);
    }
    accumulator
}

/// Returns an iterator with all the original items, but in reverse order
pub fn reverse<I: DoubleEndedIterator>(iter: I) -> impl Iterator<Item = I::Item> {
    ReverseIterator::new(iter)
}

struct ReverseIterator<I> {
    iter: I,
}

impl<I> ReverseIterator<I> {
    fn new(iter: I) -> Self {
        Self { iter }
    }
}

impl<I: DoubleEndedIterator> Iterator for ReverseIterator<I> {
    type Item = I::Item;

    fn next(&mut self) -> Option<Self::Item> {
        self.iter.next_back()
    }
}