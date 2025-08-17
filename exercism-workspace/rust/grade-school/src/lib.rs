use std::collections::HashMap;

pub struct School {
    students: HashMap<u32, Vec<String>>,
}

impl School {
    pub fn new() -> Self {
        School {
            students: HashMap::new(),
        }
    }

    pub fn add(&mut self, grade: u32, student: &str) {
        let student_name = student.to_string();
        
        // Check if student already exists in any grade
        for students_in_grade in self.students.values() {
            if students_in_grade.contains(&student_name) {
                // Student already exists, don't add again
                return;
            }
        }
        
        // Add student to the specified grade
        let grade_students = self.students.entry(grade).or_insert_with(Vec::new);
        if !grade_students.contains(&student_name) {
            grade_students.push(student_name);
            grade_students.sort();
        }
    }

    pub fn grades(&self) -> Vec<u32> {
        let mut grades: Vec<u32> = self.students.keys()
            .filter(|&grade| !self.students[grade].is_empty())
            .copied()
            .collect();
        grades.sort();
        grades
    }

    pub fn grade(&self, grade: u32) -> Vec<String> {
        self.students.get(&grade)
            .cloned()
            .unwrap_or_default()
    }
}