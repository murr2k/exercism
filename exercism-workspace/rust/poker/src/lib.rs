use std::cmp::Ordering;
use std::collections::HashMap;

#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
enum Suit {
    Spades,
    Hearts,
    Diamonds,
    Clubs,
}

impl From<char> for Suit {
    fn from(c: char) -> Self {
        match c {
            'S' => Suit::Spades,
            'H' => Suit::Hearts,
            'D' => Suit::Diamonds,
            'C' => Suit::Clubs,
            _ => panic!("Invalid suit: {}", c),
        }
    }
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
struct Rank(u8);

impl From<&str> for Rank {
    fn from(s: &str) -> Self {
        match s {
            "A" => Rank(14),
            "K" => Rank(13),
            "Q" => Rank(12),
            "J" => Rank(11),
            "10" => Rank(10),
            n => Rank(n.parse().expect("Invalid rank")),
        }
    }
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
struct Card {
    rank: Rank,
    suit: Suit,
}

impl From<&str> for Card {
    fn from(s: &str) -> Self {
        let len = s.len();
        let (rank_str, suit_str) = s.split_at(len - 1);
        let rank = Rank::from(rank_str);
        let suit = Suit::from(suit_str.chars().next().unwrap());
        Card { rank, suit }
    }
}

#[derive(Debug, PartialEq, Eq, PartialOrd, Ord, Clone)]
enum HandRank {
    HighCard(Vec<Rank>),
    OnePair(Rank, Vec<Rank>), // pair rank, other cards
    TwoPair(Rank, Rank, Rank), // high pair, low pair, kicker
    ThreeOfAKind(Rank, Vec<Rank>), // triple rank, other cards
    Straight(Rank), // high card
    Flush(Vec<Rank>), // cards in descending order
    FullHouse(Rank, Rank), // triple rank, pair rank
    FourOfAKind(Rank, Rank), // quad rank, kicker
    StraightFlush(Rank), // high card
}

#[derive(Debug)]
struct Hand {
    cards: Vec<Card>,
    rank: HandRank,
}

impl Hand {
    fn new(cards_str: &str) -> Self {
        let mut cards: Vec<Card> = cards_str.split_whitespace().map(Card::from).collect();
        cards.sort_by_key(|c| c.rank);
        
        let rank = Self::calculate_rank(&cards);
        Hand { cards, rank }
    }
    
    fn calculate_rank(cards: &[Card]) -> HandRank {
        let mut rank_counts = HashMap::new();
        for card in cards {
            *rank_counts.entry(card.rank).or_insert(0) += 1;
        }
        
        let is_flush = cards.iter().all(|c| c.suit == cards[0].suit);
        let is_straight = Self::is_straight(cards);
        
        let mut counts: Vec<(usize, Rank)> = rank_counts.into_iter().map(|(r, c)| (c, r)).collect();
        counts.sort_by(|a, b| b.cmp(a)); // Sort by count desc, then rank desc
        
        let sorted_ranks: Vec<Rank> = cards.iter().map(|c| c.rank).collect::<Vec<_>>().into_iter().rev().collect();
        
        match (is_flush, is_straight, &counts[..]) {
            (true, Some(high), _) => HandRank::StraightFlush(high),
            (_, _, [(4, quad), (1, kicker)]) => HandRank::FourOfAKind(*quad, *kicker),
            (_, _, [(3, triple), (2, pair)]) => HandRank::FullHouse(*triple, *pair),
            (true, None, _) => HandRank::Flush(sorted_ranks),
            (_, Some(high), _) => HandRank::Straight(high),
            (_, _, [(3, triple), (1, _), (1, _)]) => {
                let mut other_cards: Vec<Rank> = counts.iter().skip(1).map(|(_, r)| *r).collect();
                other_cards.sort_by(|a, b| b.cmp(a));
                HandRank::ThreeOfAKind(*triple, other_cards)
            },
            (_, _, [(2, high_pair), (2, low_pair), (1, kicker)]) => {
                let (high, low) = if high_pair > low_pair { (*high_pair, *low_pair) } else { (*low_pair, *high_pair) };
                HandRank::TwoPair(high, low, *kicker)
            },
            (_, _, [(2, pair), (1, _), (1, _), (1, _)]) => {
                let mut other_cards: Vec<Rank> = counts.iter().skip(1).map(|(_, r)| *r).collect();
                other_cards.sort_by(|a, b| b.cmp(a));
                HandRank::OnePair(*pair, other_cards)
            },
            _ => HandRank::HighCard(sorted_ranks),
        }
    }
    
    fn is_straight(cards: &[Card]) -> Option<Rank> {
        let ranks: Vec<u8> = cards.iter().map(|c| c.rank.0).collect();
        
        // Check for ace-low straight (A, 2, 3, 4, 5)
        if ranks == vec![2, 3, 4, 5, 14] {
            return Some(Rank(5)); // 5 is the high card in ace-low straight
        }
        
        // Check for regular straight
        for i in 1..ranks.len() {
            if ranks[i] != ranks[i-1] + 1 {
                return None;
            }
        }
        
        Some(cards.last().unwrap().rank)
    }
}

impl PartialEq for Hand {
    fn eq(&self, other: &Self) -> bool {
        self.rank == other.rank
    }
}

impl Eq for Hand {}

impl PartialOrd for Hand {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        Some(self.cmp(other))
    }
}

impl Ord for Hand {
    fn cmp(&self, other: &Self) -> Ordering {
        self.rank.cmp(&other.rank)
    }
}

/// Given a list of poker hands, return a list of those hands which win.
///
/// Note the type signature: this function should return _the same_ reference to
/// the winning hand(s) as were passed in, not reconstructed strings which happen to be equal.
pub fn winning_hands<'a>(hands: &[&'a str]) -> Vec<&'a str> {
    if hands.is_empty() {
        return vec![];
    }
    
    let mut hand_pairs: Vec<(&str, Hand)> = hands.iter().map(|&h| (h, Hand::new(h))).collect();
    
    // Sort by hand rank in descending order
    hand_pairs.sort_by(|a, b| b.1.cmp(&a.1));
    
    // Get the best hand rank
    let best_rank = hand_pairs[0].1.rank.clone();
    
    // Return all hands with the best rank
    hand_pairs.into_iter()
        .take_while(|(_, hand)| hand.rank == best_rank)
        .map(|(hand_str, _)| hand_str)
        .collect()
}