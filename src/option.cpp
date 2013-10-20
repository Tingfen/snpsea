#include "ezOptionParser.h"
#include "snpspec.h"

using namespace ez;

void Usage(ezOptionParser & opt)
{
    std::string usage;
    opt.getUsage(usage);
    std::cout << usage;
};

int main(int argc, const char * argv[])
{
    ezOptionParser opt;

    opt.overview = "SNPspec: an efficient statistical assessment for"
                   " enrichment\n"
                   "of continuous or binary gene annotations within disease"
                   " loci."
                   "\n======================================================="
                   "======";
    opt.syntax = "snpspec [OPTIONS]";
    opt.example =
        "  1. Condition each column in --gene-matrix on the columns listed\n"
        "     in the --condition file.\n"
        "  2. Test each column in --gene-matrix for enrichment of genes\n"
        "     within SNP intervals provided in --snp-intervals.\n"
        "  3. Replicate the test with the null matched SNP sets\n"
        "     sampled from: --null-snps\n"
        "     for the specified number of iterations: --max-iterations\n"
        "     and stop testing a column after --min-observations null SNP\n"
        "     sets with higher scores are observed.\n\n"
        "snpspec --snps file.txt               \\ # or   --snps random20 \n"
        "        --gene-matrix file.gct.gz     \\\n"
        "        --null-snps file.txt          \\\n"
        "        --snp-intervals file.bed.gz   \\\n"
        "        --gene-intervals file.bed.gz  \\\n"
        "        --condition file.txt          \\\n"
        "        --out folder                  \\\n"
        "        --slop 250e3                  \\\n"
        "        --threads 2                   \\\n"
        "        --null-snpsets 100            \\\n"
        "        --min-observations 25         \\\n"
        "        --max-iterations 1e6\n\n";
    opt.footer =
        "SNPspec v0.1  Copyright (C) 2013 Kamil Slowikowski"
        " <slowikow@broadinstitute.org>\n"
        "This program is free and without warranty under the MIT license.\n\n";

    opt.add(
        "", // Default.
        0, // Required?
        0, // Number of args expected.
        0, // Delimiter if expecting multiple args.
        "Display usage instructions.", // Help description.
        "-h",    // Flag token.
        "--help" // Flag token.
    );

    opt.add(
        "", // Default.
        0, // Required?
        0, // Number of args expected.
        0, // Delimiter if expecting multiple args.
        "Display version and exit.\n\n", // Help description.
        "-v",       // Flag token.
        "--version" // Flag token.
    );

    opt.add(
        "", // Default.
        1, // Required?
        -1, // Number of args expected.
        ',', // Delimiter if expecting multiple args.
        "One ore more text files separated by a comma. Each file must contain"
        " SNP identifiers in the first column.\n"
        "Instead of a file name, you may use 'randomN' with an integer N for"
        " a random SNP list of length N.\n\n",
        "--snps" // Flag token.
    );

    opt.add(
        "", // Default.
        1, // Required?
        1, // Number of args expected.
        0, // Delimiter if expecting multiple args.
        "Gene matrix file in GCT format. The Name column must contain the"
        " same gene identifiers as in --gene-intervals.\n\n",
        "--gene-matrix" // Flag token.
    );

    opt.add(
        "", // Default.
        1, // Required?
        1, // Number of args expected.
        0, // Delimiter if expecting multiple args.
        "BED file with gene intervals. The fourth column must contain the"
        " same gene identifiers as in --gene-matrix.\n\n",
        "--gene-intervals" // Flag token.
    );

    opt.add(
        "", // Default.
        1, // Required?
        1, // Number of args expected.
        0, // Delimiter if expecting multiple args.
        "BED file with all known SNP intervals. The fourth column must"
        " contain the same SNP identifiers as in --snps and --null-snps.\n\n",
        "--snp-intervals" // Flag token.
    );

    opt.add(
        "", // Default.
        1, // Required?
        1, // Number of args expected.
        0, // Delimiter if expecting multiple args.
        "Text file with SNP identifiers to sample when generating null"
        " matched or random SNP sets. These SNPs must be a subset of"
        " --snp-intervals.\n\n",
        "--null-snps" // Flag token.
    );

    opt.add(
        "", // Default.
        1, // Required?
        1, // Number of args expected.
        0, // Delimiter if expecting multiple args.
        "Create output files in this directory.\n\n", // Help description.
        "--out" // Flag token.
    );

    opt.add(
        "", // Default.
        0, // Required?
        1, // Number of args expected.
        0, // Delimiter if expecting multiple args.
        "Text file with a list of columns in --gene-matrix to condition on"
        " before calculating p-values. Each column in --gene-matrix is"
        " projected onto each column listed in this file and its projection"
        " is subtracted.\n\n",
        "--condition" // Flag token.
    );

    ezOptionValidator* vU8 = new ezOptionValidator(ezOptionValidator::U8);
    opt.add(
        "250000", // Default.
        0, // Required?
        1, // Number of args expected.
        0, // Delimiter if expecting multiple args.
        "If a SNP overlaps no gene intervals, extend the SNP interval this"
        " many nucleotides further and try again.\n[default: 250000]\n\n",
        "--slop", // Flag token.
        vU8
    );

    auto gt1 = new ezOptionValidator("s4", "ge", "1");
    opt.add(
        "1", // Default.
        0, // Required?
        1, // Number of args expected.
        0, // Delimiter if expecting multiple args.
        "Number of threads to use.\n[default: 1]\n\n",
        "--threads", // Flag token.
        gt1
    );

    auto gt0 = new ezOptionValidator("s4", "ge", "0");
    opt.add(
        "10", // Default.
        0, // Required?
        1, // Number of args expected.
        0, // Delimiter if expecting multiple args.
        "Test this many null matched SNP sets, so you can compare"
        " your results to a distribution of null results.\n[default: 10]\n\n",
        "--null-snpsets", // Flag token.
        gt0
    );

    opt.add(
        "25", // Default.
        0, // Required?
        1, // Number of args expected.
        0, // Delimiter if expecting multiple args.
        "Stop testing a column in --gene-matrix after observing this many"
        " null SNP sets with specificity scores greater or equal to those"
        " obtained with the SNP set in --snps. Increase this value to obtain"
        " more accurate p-values.\n[default: 25]\n\n",
        "--min-observations", // Flag token.
        gt1
    );

    opt.add(
        "1000", // Default.
        0, // Required?
        1, // Number of args expected.
        0, // Delimiter if expecting multiple args.
        "Maximum number of null SNP sets tested against each column in"
        " --gene-matrix. Increase this value to resolve smaller p-values."
        "\n[default: 1000]\n\n",
        "--max-iterations", // Flag token.
        gt1
    );

    // Read the options.
    opt.parse(argc, argv);

    if (opt.isSet("-h")) {
        Usage(opt);
        return 1;
    }

    std::vector<std::string> badOptions;
    int i;
    if (!opt.gotRequired(badOptions)) {
        Usage(opt);
        for (i = 0; i < badOptions.size(); ++i) {
            std::cerr << "ERROR: Missing required option "
                      << badOptions[i] << ".\n\n";
        }
        return 1;
    }

    if (!opt.gotExpected(badOptions)) {
        Usage(opt);
        for (i = 0; i < badOptions.size(); ++i) {
            std::cerr << "ERROR: Got unexpected number of arguments for "
                      << badOptions[i] << ".\n\n";
        }
        return 1;
    }

    std::vector<std::string>
    user_snpset_files;

    std::string
    gene_matrix_file,
    gene_intervals_file,
    snp_intervals_file,
    null_snps_file,
    condition_file,
    out_folder;

    opt.get("--snps")->getStrings(user_snpset_files);
    opt.get("--gene-matrix")->getString(gene_matrix_file);
    opt.get("--gene-intervals")->getString(gene_intervals_file);
    opt.get("--snp-intervals")->getString(snp_intervals_file);
    opt.get("--null-snps")->getString(null_snps_file);
    opt.get("--condition")->getString(condition_file);
    opt.get("--out")->getString(out_folder);

    // Ensure the files exist.
    for (auto f : user_snpset_files) {
        // The argument may be a filename or a string like "random20".
        if (f.find("random") == 0) {
            // Grab the number, ensure it is above zero.
            std::string::size_type sz;
            int n = std::stoi(f.substr(6), &sz);
            if (n <= 0) {
                std::cerr << "ERROR: --snps " + f << std::endl;
                std::cerr << "Must be like: random20" << std::endl;
                exit(EXIT_FAILURE);
            }
        } else {
            // Otherwise, ensure the file exists.
            assert_file_exists(f);
        }
    }
    assert_file_exists(gene_matrix_file);
    assert_file_exists(gene_intervals_file);
    assert_file_exists(snp_intervals_file);
    assert_file_exists(null_snps_file);
    // Optional.
    if (condition_file.length() > 0) {
        assert_file_exists(condition_file);
    }

    // Create the output directory.
    mkpath(out_folder);

    int
    threads,
    slop;
    
    long
    null_snpset_replicates,
    min_observations,
    max_iterations;

    opt.get("--threads")->getInt(threads);
    opt.get("--null-snpsets")->getLong(null_snpset_replicates);
    opt.get("--min-observations")->getLong(min_observations);

    double
    slop_d,
    max_iterations_d;

    // Read double so we can pass things like "1e6" and "250e3".
    opt.get("--slop")->getDouble(slop_d);
    opt.get("--max-iterations")->getDouble(max_iterations_d);

    slop = slop_d;
    max_iterations = max_iterations_d;

    if (max_iterations <= 0) {
        std::cerr << "ERROR: Invalid option: --max-iterations " 
                  << max_iterations << std::endl
                  << "This option may not exceed 1e18.\n";
        exit(EXIT_FAILURE);
    }

    if (min_observations >= max_iterations || min_observations <= 0) {
        std::cerr << "ERROR: Invalid option: --min-observations " 
                  << min_observations << std::endl;
        exit(EXIT_FAILURE);
    }

    // Export all of the options used.
    //opt.exportFile(out_folder + "/args.txt", true);

    // Run the analysis.
    snpspec(
        user_snpset_files,
        gene_matrix_file,
        gene_intervals_file,
        snp_intervals_file,
        null_snps_file,
        condition_file,
        out_folder,
        slop,
        threads,
        null_snpset_replicates,
        min_observations,
        max_iterations
    );

    return 0;
}