pub fn encode(n: u64) -> String {
    if n == 0 {
        return "zero".to_string();
    }
    
    let ones = [
        "", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine",
        "ten", "eleven", "twelve", "thirteen", "fourteen", "fifteen", "sixteen",
        "seventeen", "eighteen", "nineteen"
    ];
    
    let tens = [
        "", "", "twenty", "thirty", "forty", "fifty", "sixty", "seventy", "eighty", "ninety"
    ];
    
    let scale = [
        "", "thousand", "million", "billion", "trillion", "quadrillion", "quintillion"
    ];
    
    fn three_digit_group(n: u64, ones: &[&str], tens: &[&str]) -> String {
        let mut parts = Vec::new();
        
        let hundreds = n / 100;
        let remainder = n % 100;
        
        if hundreds > 0 {
            parts.push(format!("{} hundred", ones[hundreds as usize]));
        }
        
        if remainder >= 20 {
            let ten = remainder / 10;
            let one = remainder % 10;
            if one > 0 {
                parts.push(format!("{}-{}", tens[ten as usize], ones[one as usize]));
            } else {
                parts.push(tens[ten as usize].to_string());
            }
        } else if remainder > 0 {
            parts.push(ones[remainder as usize].to_string());
        }
        
        parts.join(" ")
    }
    
    let mut groups = Vec::new();
    let mut num = n;
    let mut scale_index = 0;
    
    while num > 0 {
        let group = num % 1000;
        if group > 0 {
            let group_str = three_digit_group(group, &ones, &tens);
            if scale_index > 0 {
                groups.push(format!("{} {}", group_str, scale[scale_index]));
            } else {
                groups.push(group_str);
            }
        }
        num /= 1000;
        scale_index += 1;
    }
    
    groups.reverse();
    groups.join(" ")
}