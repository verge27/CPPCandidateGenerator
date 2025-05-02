# C++ Custom Password Candidate Generator for JtR/Hashcat

## Overview

This C++ tool is designed specifically for **red team operators** and **penetration testers** to generate highly targeted and customized password candidate lists. It **does not perform any password cracking itself**. Instead, it intelligently generates potential passwords based on provided inputs and prints them to standard output (`stdout`), ready to be piped directly into powerful cracking tools like **John the Ripper (JtR)** or **Hashcat**.

The primary goal is to leverage C++ for complex candidate generation logic (e.g., combining target-specific information, applying unique patterns) while offloading the computationally intensive hashing and comparison operations to optimized tools like JtR/Hashcat, which excel at utilizing CPU and GPU resources.

## Why Use This Tool?

* **Targeted Generation:** Create wordlists tailored to a specific target using reconnaissance data (company names, project names, usernames, locations, etc.), significantly increasing the probability of cracking passwords compared to generic lists.
* **Complementary to JtR/Hashcat:** Designed explicitly to work *with* industry-standard crackers, leveraging their speed and extensive hash-type support. It fills a gap by allowing more complex *candidate generation* than standard rule engines might easily permit.
* **Extensible Logic:** Written in C++, allowing developers to easily add sophisticated generation algorithms (e.g., custom permutations, Markov chains, context-free grammars, advanced date manipulations) that go beyond typical rule sets.
* **Efficiency:** By generating unique candidates (`std::set` is used internally) and piping them directly, it avoids creating massive intermediate wordlist files and focuses the cracker's effort on relevant guesses.
* **Red Team Focus:** Ideal for scenarios where default wordlists and rules fail, requiring password guesses derived from specific intelligence gathered during an engagement.

## Features (Current Implementation)

* **Base Wordlist Integration:** Reads a standard wordlist as a starting point.
* **Target-Specific Info Combination:** Combines base words with target-specific information (provided in a separate file) using common patterns (e.g., `base+info`, `info+base`, `base+info+year`, `info+base+year`, basic capitalization).
* **Simple Leetspeak:** Applies common character substitutions (e.g., `e->3`, `a->@`, `s->$`).
* **Standard Output Piping:** Outputs generated candidates directly to `stdout`, ready for piping.
* **Duplicate Prevention:** Ensures only unique candidates are outputted.
* **Extensible:** Designed with functions for different strategies, making it easy to add more.

## Dependencies

1.  **C++ Compiler:** A modern C++ compiler supporting C++11 or later (e.g., `g++`, `Clang`). This is needed to compile the source code.
2.  **Standard C++ Library:** Required for basic I/O, strings, vectors, sets, algorithms. (This is typically included with the C++ compiler).
3.  **Password Cracker (External):** **John the Ripper** or **Hashcat** (or a similar tool that accepts candidates via `stdin`) is **required** to actually perform the password cracking using the output of this generator. This tool *only* generates candidates, it does not crack hashes.

## Compilation

Navigate to the directory containing the source code (`candidate_generator.cpp`) using your terminal and compile using g++ (or your preferred C++ compiler):

```bash
g++ candidate_generator.cpp -o candidate_generator -std=c++11 -O2
-std=c++11: Ensures C++11 features are enabled.-o candidate_generator: Specifies the output executable name (you can change candidate_generator if desired).-O2: (Optional) Applies level 2 compiler optimizations, which can improve performance.This will create an executable file named candidate_generator in the current directory.UsageThe generator takes one mandatory argument (the path to a base wordlist) and one optional argument (the path to a file containing target-specific information). It prints the generated password candidates to standard output, one candidate per line.Basic Syntax:./candidate_generator <base_wordlist_path> [target_info_path]
<base_wordlist_path>: Path to the file containing base words (e.g., common_words.txt).[target_info_path]: (Optional) Path to the file containing target-specific strings (e.g., company_data.txt).Piping into Cracking Tools (Primary Use Case):The real power comes from piping the output directly into John the Ripper or Hashcat.Example with John the Ripper:Cracking NTLM hashes (--format=NT) from ntlm_hashes.txt:./candidate_generator base_words.txt target_info.txt | john --stdin --format=NT ntlm_hashes.txt
Cracking Linux SHA512-crypt hashes (--format=sha512crypt) from shadow.txt:./candidate_generator base_words.txt target_info.txt | john --stdin --format=sha512crypt shadow.txt
--stdin: Tells John to read password candidates from standard input instead of a wordlist file.--format=<type>: Specifies the hash type JtR should expect. Consult JtR documentation for correct format names.Example with Hashcat:Cracking NTLM hashes (Mode -m 1000) from ntlm_hashes.txt:./candidate_generator base_words.txt target_info.txt | hashcat -m 1000 -a 0 ntlm_hashes.txt
Cracking bcrypt hashes (Mode -m 3200) from bcrypt_hashes.txt:./candidate_generator base_words.txt target_info
