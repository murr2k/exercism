#[derive(Debug, Eq)]
pub struct CustomSet<T> {
    elements: Vec<T>,
}

impl<T> CustomSet<T>
where
    T: Clone + PartialEq + Ord,
{
    pub fn new(input: &[T]) -> Self {
        let mut elements = Vec::new();
        for element in input {
            if !elements.contains(element) {
                elements.push(element.clone());
            }
        }
        elements.sort();
        CustomSet { elements }
    }

    pub fn contains(&self, element: &T) -> bool {
        self.elements.contains(element)
    }

    pub fn add(&mut self, element: T) {
        if !self.elements.contains(&element) {
            self.elements.push(element);
            self.elements.sort();
        }
    }

    pub fn is_subset(&self, other: &Self) -> bool {
        self.elements.iter().all(|element| other.contains(element))
    }

    pub fn is_empty(&self) -> bool {
        self.elements.is_empty()
    }

    pub fn is_disjoint(&self, other: &Self) -> bool {
        self.elements.iter().all(|element| !other.contains(element))
    }

    #[must_use]
    pub fn intersection(&self, other: &Self) -> Self {
        let mut result = Vec::new();
        for element in &self.elements {
            if other.contains(element) {
                result.push(element.clone());
            }
        }
        result.sort();
        CustomSet { elements: result }
    }

    #[must_use]
    pub fn difference(&self, other: &Self) -> Self {
        let mut result = Vec::new();
        for element in &self.elements {
            if !other.contains(element) {
                result.push(element.clone());
            }
        }
        result.sort();
        CustomSet { elements: result }
    }

    #[must_use]
    pub fn union(&self, other: &Self) -> Self {
        let mut result = self.elements.clone();
        for element in &other.elements {
            if !result.contains(element) {
                result.push(element.clone());
            }
        }
        result.sort();
        CustomSet { elements: result }
    }
}

impl<T> PartialEq for CustomSet<T>
where
    T: PartialEq,
{
    fn eq(&self, other: &Self) -> bool {
        self.elements == other.elements
    }
}