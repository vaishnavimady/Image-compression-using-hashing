#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <functional>  // Required for std::hash
#include <dirent.h>
#include <unordered_map>

std::string compressRLE(const std::string& input)
{
    std::string compressedData;
    int count = 1;
    char prevChar = input[0];

    for (size_t i = 1; i < input.size(); ++i) {
        if (input[i] == prevChar) {
            ++count;
        }
        else {
            compressedData += std::to_string(count) + prevChar;
            count = 1;
            prevChar = input[i];
        }
    }

    compressedData += std::to_string(count) + prevChar;

    return compressedData;
}

std::size_t generateHashCode(const std::string& input)
{
    // Hash the input string using std::hash
    std::hash<std::string> hasher;
    return hasher(input);
}

void compressImageFile(const std::string& inputFile, const std::string& outputFile, std::unordered_map<std::size_t, std::vector<std::string>>& hashCodesMap)
{
    std::ifstream imageStream(inputFile);
    if (!imageStream) {
        std::cerr << "Error opening input file: " << inputFile << std::endl;
        return;
    }

    std::string image((std::istreambuf_iterator<char>(imageStream)),
        std::istreambuf_iterator<char>());

    std::string compressedImage = compressRLE(image);

    std::size_t hashCode = generateHashCode(compressedImage);

    std::ofstream compressedStream(outputFile);
    if (!compressedStream) {
        std::cerr << "Error opening output file: " << outputFile << std::endl;
        return;
    }

    compressedStream << compressedImage;
    compressedStream.close();

    std::cout << "Image compressed and saved to: " << outputFile << std::endl;
    std::cout << "Hash code: " << hashCode << std::endl;

    // Check for repeating hash codes and update the map
    hashCodesMap[hashCode].push_back(inputFile);
}

void processImagesInFolder(const std::string& folderPath)
{
    DIR* directory = opendir(folderPath.c_str());
    if (!directory) {
        std::cerr << "Error opening folder: " << folderPath << std::endl;
        return;
    }

    dirent* entry;
    std::unordered_map<std::size_t, std::vector<std::string>> hashCodesMap;
    
    while ((entry = readdir(directory))) {
        if (entry->d_type == DT_REG) { // Regular file
            std::string fileName = entry->d_name;
            std::string inputFilePath = folderPath + "/" + fileName;
            std::string outputFilePath = folderPath + "/" + fileName + ".compressed.txt";

            compressImageFile(inputFilePath, outputFilePath, hashCodesMap);
        }
    }

    closedir(directory);

    // Display repeating hash codes and corresponding file names
    std::cout << "Repeating Hash Codes:" << std::endl;
    for (const auto& entry : hashCodesMap) {
        if (entry.second.size() > 1) {
            std::cout << "Hash Code: " << entry.first << std::endl;
            std::cout << "File Names: ";
            for (const auto& fileName : entry.second) {
                std::cout << fileName << " ";
            }
            std::cout << std::endl;
        }
    }
}

int main()
{
    std::string folderPath = "C:\\Users\\hp\\pictures";

    processImagesInFolder(folderPath);

    return 0;
}
