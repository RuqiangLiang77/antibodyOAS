use std::env;
use std::fs::File;
use std::io::{self, BufRead};
use std::path::Path;

const MAX_LENGTH: usize = 70;
const MAX_LINE: usize = 128;
const AMINO_ACIDS: usize = 21;

const AMINO_ACID_LIST: [char; AMINO_ACIDS] = [
    'A','C','D','E','F','G','H','I','K','L',
    'M','N','P','Q','R','S','T','V','W','Y','X'
];

fn aa_index(aa: char) -> Option<usize> {
    AMINO_ACID_LIST.iter().position(|&c| c == aa)
}

fn process_file(
    filename: &str,
    length_counts: &mut [usize; MAX_LENGTH + 1],
    distributions: &mut [[[usize; AMINO_ACIDS]; MAX_LENGTH]; MAX_LENGTH + 1],
) {
    if let Ok(file) = File::open(filename) {
        for line in io::BufReader::new(file).lines().flatten() {
            let line = line.trim();
            let length = line.len();
            if length == 0 || length > MAX_LENGTH {
                continue;
            }

            length_counts[length] += 1;
            for (i, ch) in line.chars().enumerate() {
                if let Some(idx) = aa_index(ch) {
                    distributions[length][i][idx] += 1;
                }
            }
        }
    } else {
        eprintln!("Error: Could not open file {}", filename);
        std::process::exit(1);
    }
}

fn print_results(
    cdr_name: &str,
    length_counts: &[usize; MAX_LENGTH + 1],
    distributions: &[[[usize; AMINO_ACIDS]; MAX_LENGTH]; MAX_LENGTH + 1],
) {
    println!("Results for {}:", cdr_name);
    let total_records: usize = length_counts.iter().sum();
    println!("Total records: {}", total_records);

    for len in 1..=MAX_LENGTH {
        if length_counts[len] == 0 {
            continue;
        }
        println!(
            "Length {}: {} ({:.4}%) records",
            len,
            length_counts[len],
            100.0 * length_counts[len] as f64 / total_records as f64
        );
        for pos in 0..len {
            print!("  Position {}: ", pos + 1);
            for aa in 0..AMINO_ACIDS {
                let count = distributions[len][pos][aa];
                if count > 0 {
                    print!(
                        "{}({:.4}%) ",
                        AMINO_ACID_LIST[aa],
                        100.0 * count as f64 / length_counts[len] as f64
                    );
                }
            }
            println!();
        }
    }
    println!();
}

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() < 2 || args.len() > 6 {
        eprintln!("Usage: {} <cdr1.txt> <cdr2.txt> <cdr3.txt> [<cdr4.txt> <cdr5.txt>]", args[0]);
        std::process::exit(1);
    }

    let names = ["CDR1", "CDR2", "CDR3", "CDR4", "CDR5"];

    for (i, filename) in args.iter().skip(1).enumerate() {
        let mut length_counts = [0usize; MAX_LENGTH + 1];
        let mut distributions = [[[0usize; AMINO_ACIDS]; MAX_LENGTH]; MAX_LENGTH + 1];

        process_file(filename, &mut length_counts, &mut distributions);
        print_results(names[i], &length_counts, &distributions);
    }
}
