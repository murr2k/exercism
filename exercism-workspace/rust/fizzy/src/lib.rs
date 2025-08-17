use std::fmt::Display;

/// A Matcher is a single rule of fizzbuzz: given a function on T, should
/// a word be substituted in? If yes, which word?
pub struct Matcher<T> {
    matcher: Box<dyn Fn(T) -> bool>,
    substitution: String,
}

impl<T> Matcher<T> {
    pub fn new<F, S>(matcher: F, subs: S) -> Matcher<T> 
    where
        F: Fn(T) -> bool + 'static,
        S: ToString,
    {
        Matcher {
            matcher: Box::new(matcher),
            substitution: subs.to_string(),
        }
    }
}

/// A Fizzy is a set of matchers, which may be applied to an iterator.
pub struct Fizzy<T> {
    matchers: Vec<Matcher<T>>,
}

impl<T> Fizzy<T> {
    pub fn new() -> Self {
        Fizzy {
            matchers: Vec::new(),
        }
    }

    #[must_use]
    pub fn add_matcher(mut self, matcher: Matcher<T>) -> Self {
        self.matchers.push(matcher);
        self
    }

    /// map this fizzy onto every element of an iterator, returning a new iterator
    pub fn apply<I>(self, iter: I) -> impl Iterator<Item = String> 
    where
        I: Iterator<Item = T>,
        T: Display + Copy + 'static,
    {
        iter.map(move |item| {
            let mut result = String::new();
            for matcher in &self.matchers {
                if (matcher.matcher)(item) {
                    result.push_str(&matcher.substitution);
                }
            }
            if result.is_empty() {
                item.to_string()
            } else {
                result
            }
        })
    }
}

/// convenience function: return a Fizzy which applies the standard fizz-buzz rules
pub fn fizz_buzz<T>() -> Fizzy<T> 
where
    T: Copy + Default + PartialEq + From<u8> + std::ops::Rem<Output = T>,
{
    Fizzy::new()
        .add_matcher(Matcher::new(|n: T| n % T::from(3) == T::default(), "fizz"))
        .add_matcher(Matcher::new(|n: T| n % T::from(5) == T::default(), "buzz"))
}