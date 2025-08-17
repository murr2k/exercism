use std::collections::HashMap;

#[derive(Debug, Default)]
struct TeamStats {
    matches_played: u32,
    wins: u32,
    draws: u32,
    losses: u32,
}

impl TeamStats {
    fn points(&self) -> u32 {
        self.wins * 3 + self.draws
    }
}

pub fn tally(match_results: &str) -> String {
    let mut teams: HashMap<String, TeamStats> = HashMap::new();
    
    // Parse match results
    for line in match_results.lines() {
        if line.trim().is_empty() {
            continue;
        }
        
        let parts: Vec<&str> = line.split(';').collect();
        if parts.len() != 3 {
            continue;
        }
        
        let team1 = parts[0].trim();
        let team2 = parts[1].trim();
        let result = parts[2].trim();
        
        // Ensure both teams exist in the map
        teams.entry(team1.to_string()).or_default();
        teams.entry(team2.to_string()).or_default();
        
        // Update stats based on result
        match result {
            "win" => {
                // team1 wins, team2 loses
                teams.get_mut(team1).unwrap().wins += 1;
                teams.get_mut(team1).unwrap().matches_played += 1;
                teams.get_mut(team2).unwrap().losses += 1;
                teams.get_mut(team2).unwrap().matches_played += 1;
            }
            "loss" => {
                // team1 loses, team2 wins
                teams.get_mut(team1).unwrap().losses += 1;
                teams.get_mut(team1).unwrap().matches_played += 1;
                teams.get_mut(team2).unwrap().wins += 1;
                teams.get_mut(team2).unwrap().matches_played += 1;
            }
            "draw" => {
                // Both teams draw
                teams.get_mut(team1).unwrap().draws += 1;
                teams.get_mut(team1).unwrap().matches_played += 1;
                teams.get_mut(team2).unwrap().draws += 1;
                teams.get_mut(team2).unwrap().matches_played += 1;
            }
            _ => {
                // Invalid result, skip
                continue;
            }
        }
    }
    
    // Collect and sort teams
    let mut team_list: Vec<(&String, &TeamStats)> = teams.iter().collect();
    team_list.sort_by(|a, b| {
        // First sort by points (descending), then by name (ascending)
        b.1.points().cmp(&a.1.points()).then_with(|| a.0.cmp(b.0))
    });
    
    // Format output
    let mut result = vec!["Team                           | MP |  W |  D |  L |  P".to_string()];
    
    for (team_name, stats) in team_list {
        let line = format!(
            "{:<30} | {:>2} | {:>2} | {:>2} | {:>2} | {:>2}",
            team_name,
            stats.matches_played,
            stats.wins,
            stats.draws,
            stats.losses,
            stats.points()
        );
        result.push(line);
    }
    
    result.join("\n")
}