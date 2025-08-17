use std::collections::{HashMap, HashSet};

/// `InputCellId` is a unique identifier for an input cell.
#[derive(Clone, Copy, Debug, PartialEq, Eq, Hash)]
pub struct InputCellId(usize);

/// `ComputeCellId` is a unique identifier for a compute cell.
#[derive(Clone, Copy, Debug, PartialEq, Eq, Hash)]
pub struct ComputeCellId(usize);

#[derive(Clone, Copy, Debug, PartialEq, Eq, Hash)]
pub struct CallbackId(usize);

#[derive(Clone, Copy, Debug, PartialEq, Eq, Hash)]
pub enum CellId {
    Input(InputCellId),
    Compute(ComputeCellId),
}

#[derive(Debug, PartialEq, Eq)]
pub enum RemoveCallbackError {
    NonexistentCell,
    NonexistentCallback,
}

struct InputCell<T> {
    value: T,
}

struct ComputeCell<'a, T> {
    dependencies: Vec<CellId>,
    compute_func: Box<dyn Fn(&[T]) -> T + 'a>,
    value: T,
    callbacks: HashMap<CallbackId, Box<dyn FnMut(T) + 'a>>,
}

pub struct Reactor<'a, T> {
    input_cells: HashMap<InputCellId, InputCell<T>>,
    compute_cells: HashMap<ComputeCellId, ComputeCell<'a, T>>,
    next_input_id: usize,
    next_compute_id: usize,
    next_callback_id: usize,
}

impl<'a, T: Copy + PartialEq> Reactor<'a, T> {
    pub fn new() -> Self {
        Reactor {
            input_cells: HashMap::new(),
            compute_cells: HashMap::new(),
            next_input_id: 0,
            next_compute_id: 0,
            next_callback_id: 0,
        }
    }

    pub fn create_input(&mut self, initial: T) -> InputCellId {
        let id = InputCellId(self.next_input_id);
        self.next_input_id += 1;
        self.input_cells.insert(id, InputCell { value: initial });
        id
    }

    pub fn create_compute<F: Fn(&[T]) -> T + 'a>(
        &mut self,
        dependencies: &[CellId],
        compute_func: F,
    ) -> Result<ComputeCellId, CellId> {
        // Check if all dependencies exist
        for &dep in dependencies {
            match dep {
                CellId::Input(id) if !self.input_cells.contains_key(&id) => {
                    return Err(dep);
                }
                CellId::Compute(id) if !self.compute_cells.contains_key(&id) => {
                    return Err(dep);
                }
                _ => {}
            }
        }

        // Compute initial value
        let dep_values: Vec<T> = dependencies
            .iter()
            .map(|&id| self.value(id).unwrap())
            .collect();
        let initial_value = compute_func(&dep_values);

        let id = ComputeCellId(self.next_compute_id);
        self.next_compute_id += 1;
        
        self.compute_cells.insert(
            id,
            ComputeCell {
                dependencies: dependencies.to_vec(),
                compute_func: Box::new(compute_func),
                value: initial_value,
                callbacks: HashMap::new(),
            },
        );
        
        Ok(id)
    }

    pub fn value(&self, id: CellId) -> Option<T> {
        match id {
            CellId::Input(id) => self.input_cells.get(&id).map(|cell| cell.value),
            CellId::Compute(id) => self.compute_cells.get(&id).map(|cell| cell.value),
        }
    }

    pub fn set_value(&mut self, id: InputCellId, new_value: T) -> bool {
        if let Some(cell) = self.input_cells.get_mut(&id) {
            if cell.value == new_value {
                return true; // Value unchanged, nothing to do
            }
            cell.value = new_value;
            
            // Update all compute cells and collect callbacks to fire
            let mut callbacks_to_fire = Vec::new();
            self.update_compute_cells(&mut callbacks_to_fire);
            
            // Fire callbacks after all updates are done
            for (compute_id, new_val) in callbacks_to_fire {
                if let Some(compute_cell) = self.compute_cells.get_mut(&compute_id) {
                    for callback in compute_cell.callbacks.values_mut() {
                        callback(new_val);
                    }
                }
            }
            
            true
        } else {
            false
        }
    }

    fn update_compute_cells(&mut self, callbacks_to_fire: &mut Vec<(ComputeCellId, T)>) {
        // Update compute cells in topological order
        // We use a simple approach: keep updating until no more changes
        let mut changed = true;
        let mut iterations = 0;
        const MAX_ITERATIONS: usize = 100; // Prevent infinite loops
        
        while changed && iterations < MAX_ITERATIONS {
            changed = false;
            iterations += 1;
            
            // Collect all compute cells that need to be updated
            let compute_ids: Vec<ComputeCellId> = self.compute_cells.keys().copied().collect();
            
            for compute_id in compute_ids {
                // Get dependencies and old value
                let old_value = self.compute_cells[&compute_id].value;
                let deps = self.compute_cells[&compute_id].dependencies.clone();
                
                // Get current dependency values
                let dep_values: Vec<T> = deps
                    .iter()
                    .map(|&dep_id| match dep_id {
                        CellId::Input(id) => self.input_cells[&id].value,
                        CellId::Compute(id) => self.compute_cells[&id].value,
                    })
                    .collect();
                
                // Compute new value
                let new_value = {
                    let cell = &self.compute_cells[&compute_id];
                    (cell.compute_func)(&dep_values)
                };
                
                // If value changed, update it
                if new_value != old_value {
                    self.compute_cells.get_mut(&compute_id).unwrap().value = new_value;
                    changed = true;
                    
                    // Check if we should fire callbacks for this cell
                    let has_callbacks = self.compute_cells[&compute_id].callbacks.len() > 0;
                    if has_callbacks {
                        // Only add to callbacks_to_fire if not already there OR update if value changed
                        let existing = callbacks_to_fire.iter_mut().find(|(id, _)| *id == compute_id);
                        if let Some((_, val)) = existing {
                            *val = new_value; // Update to latest value
                        } else {
                            callbacks_to_fire.push((compute_id, new_value));
                        }
                    }
                }
            }
        }
    }

    pub fn add_callback<F: FnMut(T) + 'a>(
        &mut self,
        id: ComputeCellId,
        callback: F,
    ) -> Option<CallbackId> {
        if let Some(cell) = self.compute_cells.get_mut(&id) {
            let callback_id = CallbackId(self.next_callback_id);
            self.next_callback_id += 1;
            cell.callbacks.insert(callback_id, Box::new(callback));
            Some(callback_id)
        } else {
            None
        }
    }

    pub fn remove_callback(
        &mut self,
        cell: ComputeCellId,
        callback: CallbackId,
    ) -> Result<(), RemoveCallbackError> {
        if let Some(compute_cell) = self.compute_cells.get_mut(&cell) {
            if compute_cell.callbacks.remove(&callback).is_some() {
                Ok(())
            } else {
                Err(RemoveCallbackError::NonexistentCallback)
            }
        } else {
            Err(RemoveCallbackError::NonexistentCell)
        }
    }
}