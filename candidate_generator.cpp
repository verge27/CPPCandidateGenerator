#include <iostream> // For standard I/O (cout, cerr, endl)
#include <string>   // For using std::string
#include <vector>   // For using std::vector
#include <fstream>  // For file stream operations (ifstream)
#include <sstream>  // For string stream operations (though not strictly needed in this version)
#include <algorithm> // For algorithms like std::transform, std::replace, std::all_of
#include <set>      // For using std::set to store unique candidates
#include <cctype>   // For character handling functions (isprint, toupper)
#include <stdexcept> // For standard exceptions (though not used here, good practice for future)

// --- Helper Function ---

/**
 * @brief Checks if all characters in a string are printable ASCII characters.
 * Helps filter out potentially problematic binary data read from files.
 * @param s The string to check.
 * @return true if all characters are printable, false otherwise.
 */
bool is_printable(const std::string& s) {
    // std::all_of checks if the predicate (lambda function) returns true for all elements in the range.
    return std::all_of(s.begin(), s.end(), [](unsigned char c){
        // std::isprint checks if the character is printable (including space).
        return std::isprint(c);
    });
}

// --- Generation Strategies ---

/**
 * @brief Generates password candidates by combining base words with target-specific info.
 * Includes simple concatenations, suffix additions (years, common symbols),
 * and basic capitalization variations.
 * @param base_words A vector of base words (from a general wordlist).
 * @param target_info A vector of target-specific strings (company names, locations, etc.).
 * @param candidates A reference to a set where unique generated candidates will be inserted.
 */
void generate_target_combinations(const std::vector<std::string>& base_words,
                                  const std::vector<std::string>& target_info,
                                  std::set<std::string>& candidates) {
    std::cerr << "[*] Generating target combinations..." << std::endl; // Use cerr for status messages
    // Example suffixes - easily expandable
    const std::vector<std::string> common_suffixes = {"2023", "2024", "2025", "!", "1", "123", "#"};

    // Iterate through each base word
    for (const std::string& base : base_words) {
        // Skip empty or non-printable base words
        if (!is_printable(base) || base.empty()) continue;
        candidates.insert(base); // Always include the base word itself

        // Combine with each piece of target info
        for (const std::string& info : target_info) {
            // Skip empty or non-printable target info
            if (!is_printable(info) || info.empty()) continue;

            // Simple combinations (base+info, info+base)
            candidates.insert(base + info);
            candidates.insert(info + base);

            // Combinations with common suffixes
            for (const std::string& suffix : common_suffixes) {
                candidates.insert(base + info + suffix);
                candidates.insert(info + base + suffix);
                candidates.insert(base + suffix + info); // Less common pattern, but possible
                candidates.insert(info + suffix + base); // Less common pattern, but possible

                // --- Basic Capitalization Variations ---
                // Create capitalized versions (handle empty strings safely)
                std::string cap_info = info;
                if (!cap_info.empty()) cap_info[0] = std::toupper(cap_info[0]);
                std::string cap_base = base;
                if (!cap_base.empty()) cap_base[0] = std::toupper(cap_base[0]);

                // Insert capitalized combinations
                candidates.insert(cap_base + cap_info); // CapBaseCapInfo
                candidates.insert(cap_info + cap_base); // CapInfoCapBase
                candidates.insert(cap_base + info);     // CapBaseinfo
                candidates.insert(info + cap_base);     // infoCapBase

                // Insert capitalized combinations with suffixes
                candidates.insert(cap_base + cap_info + suffix);
                candidates.insert(cap_info + cap_base + suffix);
                candidates.insert(cap_base + info + suffix);
                candidates.insert(info + cap_base + suffix);
            }
        }

        // Also combine the base word directly with suffixes
        for (const std::string& suffix : common_suffixes) {
             candidates.insert(base + suffix); // baseSuffix
             // Capitalized base word + suffix
             std::string cap_base = base;
             if (!cap_base.empty()) cap_base[0] = std::toupper(cap_base[0]);
             candidates.insert(cap_base + suffix); // CapBaseSuffix
        }
    }
    std::cerr << "[*] Finished target combinations." << std::endl;
}

/**
 * @brief Applies simple leetspeak substitutions to a list of words.
 * e->3, a->@, o->0, s->$, i->1 (case-insensitive)
 * @param input_words A vector of words to apply leetspeak to.
 * @param candidates A reference to a set where unique generated candidates (including leetspeak versions) will be inserted.
 */
void apply_leetspeak(const std::vector<std::string>& input_words,
                       std::set<std::string>& candidates) {
    std::cerr << "[*] Applying simple leetspeak..." << std::endl;
    // Iterate through each input word
    for (const std::string& word : input_words) {
        // Skip empty or non-printable words
        if (!is_printable(word) || word.empty()) continue;
        candidates.insert(word); // Ensure original word is in the set

        // Create a copy to modify for leetspeak
        std::string leet_word = word;

        // Perform substitutions (case-insensitive by replacing both lower and upper)
        std::replace(leet_word.begin(), leet_word.end(), 'e', '3');
        std::replace(leet_word.begin(), leet_word.end(), 'E', '3');
        std::replace(leet_word.begin(), leet_word.end(), 'a', '@');
        std::replace(leet_word.begin(), leet_word.end(), 'A', '@');
        std::replace(leet_word.begin(), leet_word.end(), 'o', '0');
        std::replace(leet_word.begin(), leet_word.end(), 'O', '0');
        std::replace(leet_word.begin(), leet_word.end(), 's', '$');
        std::replace(leet_word.begin(), leet_word.end(), 'S', '$');
        std::replace(leet_word.begin(), leet_word.end(), 'i', '1');
        std::replace(leet_word.begin(), leet_word.end(), 'I', '1');
        std::replace(leet_word.begin(), leet_word.end(), 't', '7'); // Example: adding 't'
        std::replace(leet_word.begin(), leet_word.end(), 'T', '7'); // Example: adding 'T'


        // Only insert the leetspeak version if it's different from the original
        if (leet_word != word) {
            candidates.insert(leet_word);
        }
        // Note: More complex rules could involve partial substitutions,
        // checking context, or using more obscure replacements.
    }
     std::cerr << "[*] Finished leetspeak." << std::endl;
}

/**
 * @brief Loads lines from a text file into a vector of strings.
 * Handles potential Windows line endings (\r\n).
 * @param path The path to the file to load.
 * @return A vector containing the lines from the file. Returns an empty vector if file cannot be opened.
 */
std::vector<std::string> load_file_lines(const std::string& path) {
    std::vector<std::string> lines;
    std::ifstream file(path); // Input file stream

    // Check if the file was opened successfully
    if (!file) {
        // Print warning to standard error
        std::cerr << "Warning: Could not open file: " << path << ". Skipping." << std::endl;
        return lines; // Return empty vector
    }

    std::string line;
    // Read the file line by line
    while (std::getline(file, line)) {
         // Remove potential trailing carriage return ('\r') from Windows files
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        // Add non-empty lines to the vector
        if (!line.empty()) {
            lines.push_back(line);
        }
    }
    // File is automatically closed when 'file' goes out of scope
    return lines;
}


// --- Main Function ---
int main(int argc, char* argv[]) {
    // --- Basic Argument Parsing ---
    // Check if at least the base wordlist path is provided
    if (argc < 2) {
        // Print usage instructions to standard error
        std::cerr << "Usage: " << argv[0] << " <base_wordlist_path> [target_info_path]" << std::endl;
        std::cerr << "Description: Generates password candidates based on input lists and prints them to stdout." << std::endl;
        std::cerr << "             Designed to be piped into password cracking tools like John the Ripper or Hashcat." << std::endl;
        std::cerr << std::endl;
        std::cerr << "Example (John the Ripper):" << std::endl;
        std::cerr << "  " << argv[0] << " common_words.txt company_info.txt | john --stdin --format=NT hashes.txt" << std::endl;
        std::cerr << std::endl;
        std::cerr << "Example (Hashcat):" << std::endl;
        std::cerr << "  " << argv[0] << " common_words.txt company_info.txt | hashcat -m 1000 -a 0 hashes.txt" << std::endl;
        return 1; // Indicate error
    }

    // Get file paths from command line arguments
    std::string base_wordlist_path = argv[1];
    // Check if the optional target info path was provided
    std::string target_info_path = (argc > 2) ? argv[2] : "";

    // --- Load Input Data ---
    std::cerr << "[*] Loading base wordlist: " << base_wordlist_path << std::endl;
    std::vector<std::string> base_words = load_file_lines(base_wordlist_path);

    std::vector<std::string> target_info;
    if (!target_info_path.empty()) {
        std::cerr << "[*] Loading target info: " << target_info_path << std::endl;
        target_info = load_file_lines(target_info_path);
    } else {
         std::cerr << "[*] No target info file provided." << std::endl;
    }

    // Exit if the base wordlist is empty (critical input)
    if (base_words.empty()) {
         std::cerr << "Error: Base wordlist is empty or could not be read from " << base_wordlist_path << "." << std::endl;
         return 1; // Indicate error
    }

    // --- Candidate Generation ---
    // Use std::set to automatically store unique candidates
    std::set<std::string> generated_candidates;

    // --- Apply Generation Strategies ---

    // 1. Start by adding all printable base words to the candidate set
    std::cerr << "[*] Initializing candidates with base words..." << std::endl;
    for(const auto& word : base_words) {
        if (is_printable(word) && !word.empty()) {
            generated_candidates.insert(word);
        }
    }

    // 2. Combine base words with target info (if provided)
    if (!target_info.empty()) {
        // This function modifies generated_candidates directly
        generate_target_combinations(base_words, target_info, generated_candidates);
    }

    // 3. Apply leetspeak rules to all candidates generated so far
    //    Convert the current set to a vector to pass to the function
    //    (avoids modifying the set while iterating, though apply_leetspeak doesn't iterate its input set)
    std::vector<std::string> current_candidates_vec(generated_candidates.begin(), generated_candidates.end());
    // This function also modifies generated_candidates directly
    apply_leetspeak(current_candidates_vec, generated_candidates);

    // --- Add calls to more generation strategies here ---
    // e.g., date variations, common keyboard walks, Markov chains, etc.
    // Example placeholder:
    // if (enable_date_generation_flag) { // Check if enabled via command line arg
    //     generate_date_variations(base_words, target_info, generated_candidates);
    // }


    // --- Output Candidates to stdout ---
    // Print status message to stderr
    std::cerr << "[*] Outputting " << generated_candidates.size() << " unique candidates to stdout..." << std::endl;
    // Iterate through the final set of unique candidates
    for (const std::string& candidate : generated_candidates) {
        // Print each candidate to standard output, followed by a newline
        std::cout << candidate << std::endl;
    }
    // Print final status message to stderr
    std::cerr << "[*] Candidate generation complete." << std::endl;


    return 0; // Indicate success
}
