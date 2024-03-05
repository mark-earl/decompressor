/*
    decompressor.cpp

    Decompresses the input file with the output of the uncompressed text.
    If multiple files, the output is separated by NULL ASCII 0 character.
*/

#include <iostream>
#include <unistd.h>
#include <archive.h>
#include <archive_entry.h>

int main(int argc, char* argv[]) {

    // allows at most one filename
    if (argc > 2) {
        std::cerr << "Usage: " << argv[0] << " [compressed-filename]\n";
        return 1;
    }

    // input filename is single argument or NULL for stdin
    char* inputFilename = nullptr;
    if (argc == 2) {
        inputFilename = argv[1];
    }

    // open the archive file for all compressions and archive formats
    archive* inputArchive = archive_read_new();
    if (!inputArchive) {
        std::cerr << argv[0] << ": error in creating input archive" << '\n';
        return 1;
    }
    // all file compressions
    int status = archive_read_support_filter_all(inputArchive);
    if (status != ARCHIVE_OK && status != ARCHIVE_WARN) {
        std::cerr << argv[0] << ": error in using archive_read_support_filter_all()" << '\n';
        return 1;
    }
    // all file archive formats
    status = archive_read_support_format_all(inputArchive);
    if (status != ARCHIVE_OK && status != ARCHIVE_WARN) {
        std::cerr << argv[0] << ": error in using archive_read_support_format_all()" << '\n';
        return 1;
    }
    // uncompressed files
    status = archive_read_support_format_raw(inputArchive);
    if (status != ARCHIVE_OK && status != ARCHIVE_WARN) {
        std::cerr << argv[0] << ": error in using archive_read_support_format_raw()" << '\n';
        return 1;
    }
    // empty files
    status = archive_read_support_format_empty(inputArchive);
    if (status != ARCHIVE_OK && status != ARCHIVE_WARN) {
        std::cerr << argv[0] << ": error in using archive_read_support_format_empty()" << '\n';
        return 1;
    }
    status = archive_read_open_filename(inputArchive, inputFilename, 16384);
    if (status != ARCHIVE_OK) {
        std::cerr << argv[0] << ": error in opening " << inputFilename << '\n';
        return 1;
    }

    // output each entry separated by a ASCII NULL ('\0')
    bool first = true;
    while (true) {

        // open the archive entry
        archive_entry* inputEntry = nullptr;
        int status = archive_read_next_header(inputArchive, &inputEntry);
        if (status == ARCHIVE_EOF) {
            break;
        }
        if (status != ARCHIVE_OK) {
            std::cerr << argv[0] << ": error in opening " << inputFilename << '\n';
            return 1;
        }

        // separate each output entry with a ASCII NULL ('\0')
        if (!first) {
            if (write(1, "", 1) == -1) {
                std::cerr << argv[0] << ": unable to write to stdout" << '\n';
                return 1;
            }
        }
        first = false;

        // copy the decompressed archive entry to stdout
        const int BUFFER_SIZE = 4096;
        char buffer[BUFFER_SIZE];
        while (true) {

            // read the decompressed block
            auto size = archive_read_data(inputArchive, buffer, BUFFER_SIZE);
            if (size < 0) {
                std::cerr << argv[0] << ": error in reading data" << '\n';
                return 1;
            }
            if (size == 0) {
               // EOF of entry
               break;
            }

            // write the decompressed block to standard output
            if (write(1, buffer, size) == -1) {
                std::cerr << argv[0] << ": unable to write to stdout" << '\n';
                return 1;
            }
        }
    }

    // free the input archive
    archive_read_free(inputArchive);

    return 0;
}
