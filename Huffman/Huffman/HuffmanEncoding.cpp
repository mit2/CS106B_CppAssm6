/**********************************************************
 * File: HuffmanEncoding.cpp
 *
 * Implementation of the functions from HuffmanEncoding.h.
 * Most (if not all) of the code that you write for this
 * assignment will go into this file.
 */

#include "HuffmanEncoding.h"
#include "HuffmanPQueue.h"
#include "HuffmanTypes.h"
#include "map.h"
#include "strlib.h"
#include <iostream>
using namespace std;

/* Function: getFrequencyTable
 * Usage: Map<ext_char, int> freq = getFrequencyTable(file);
 * --------------------------------------------------------
 * Given an input stream containing text, calculates the
 * frequencies of each character within that text and stores
 * the result as a Map from ext_chars to the number of times
 * that the character appears.
 *
 * This function will also set the frequency of the PSEUDO_EOF
 * character to be 1, which ensures that any future encoding
 * tree built from these frequencies will have an encoding for
 * the PSEUDO_EOF character.
 */
Map<ext_char, int> getFrequencyTable(istream& file) {
	// WAS EAZY TASK FOR ME
	if (file.fail()) error("Can't open input file!");
	ext_char ch;
	Map<ext_char, int> charTable;

	while ((ch = file.get()) != EOF){
		if(charTable.containsKey(ch)){
			int i = charTable.get(ch);
			charTable.put(ch, ++i);
		}else
			charTable.put(ch, 1);
	}
	// insert ending PSEUDO_EOF character
	charTable.put(PSEUDO_EOF, 1);

	
	//file.close();
	return charTable;	
}

/* Function: buildEncodingTree
 * Usage: Node* tree = buildEncodingTree(frequency);
 * --------------------------------------------------------
 * Given a map from extended characters to frequencies,
 * constructs a Huffman encoding tree from those frequencies
 * and returns a pointer to the root.
 *
 * This function can assume that there is always at least one
 * entry in the map, since the PSEUDO_EOF character will always
 * be present.
 */
Node* buildEncodingTree(Map<ext_char, int>& frequencies) {
	// WAS EAZY TASK FOR ME
	HuffmanPQueue hpqCollection; // create collection to store initial singletones

	// Crate Huffman Tree leafs
	// Iterate thru charTable Map and create singletones and insert into pqueue
	foreach (ext_char ch in frequencies){
		Node *leaf = new Node;
		leaf->character = ch;
		leaf->weight = frequencies.get(ch);
		leaf->one = NULL;
		leaf->zero = NULL;
		hpqCollection.enqueue(leaf);
	}

	// build the Huffman tree
	while(hpqCollection.size() != 1){
		//cout << "pqsize: " << hpqCollection.size() << endl;
		Node *tmp1 = hpqCollection.dequeueMin();
		Node *tmp2 = hpqCollection.dequeueMin();
		// create new root for subtrees
		Node *iNode = new Node;
		iNode->character = NOT_A_CHAR;
		iNode->weight = tmp1->weight + tmp2->weight;
		iNode->one = tmp2;
		iNode->zero = tmp1;

		// insert back into pqueue combined new tree
		hpqCollection.enqueue(iNode);
	}
	
	/*cout << "==============================" <<endl;
	buildEncodingMap(hpqCollection.peek(), "");
	cout << "==============================" <<endl;*/
/*	cout << "size: " << hpqCollection.size() << endl;
	cout << "EOF: " << frequencies.get(PSEUDO_EOF) << endl;*/


	return hpqCollection.peek();
}

/* Function: freeTree
 * Usage: freeTree(encodingTree);
 * --------------------------------------------------------
 * Deallocates all memory allocated for a given encoding
 * tree.
 */
void freeTree(Node* root) {
	// WAS EAZY TASK FOR ME
	// TODO: Freeing memory p. 524 in CS106B reader	
	if(root->one != NULL) freeTree(root->one);		// recurr on right tree branch	
	if(root->zero != NULL) freeTree(root->zero);	// recurr on left tree branch
	if(root != NULL) delete root;
}

//*********************************************************
//-------------------------------MY CUSTOM CODE------------
//*********************************************************

/* Function: getFrequencyTable
 * Usage: buildEncodingMap(encodingTree, encoding);
 * --------------------------------------------------------
 * Builds encoding Map by mapping char to it's encoding.
 */
void buildEncodingMap(Node* root, string encoding, Map<ext_char, string>& encodingMap){
	string bitPattern_1 = encoding; 
    string bitPattern_2 = encoding;
	if(root->one != NULL) buildEncodingMap(root->one, bitPattern_1.append("1"), encodingMap);
	if(root->zero != NULL) buildEncodingMap(root->zero, bitPattern_2.append("0"), encodingMap);
	if(root->character != NOT_A_CHAR){
		encodingMap.put(root->character, encoding);
	}
	
}

//*********************************************************
//-------------------------------MY CUSTOM CODE END--------
//*********************************************************


/* Function: encodeFile
 * Usage: encodeFile(source, encodingTree, output);
 * --------------------------------------------------------
 * Encodes the given file using the encoding specified by the
 * given encoding tree, then writes the result one bit at a
 * time to the specified output file.
 *
 * This function can assume the following:
 *
 *   - The encoding tree was constructed from the given file,
 *     so every character appears somewhere in the encoding
 *     tree.
 *
 *   - The output file already has the encoding table written
 *     to it, and the file cursor is at the end of the file.
 *     This means that you should just start writing the bits
 *     without seeking the file anywhere.
 */ 
void encodeFile(istream& infile, Node* encodingTree, obstream& outfile) {
	// TODO: Implement this!
	ext_char ch;
	Map<ext_char, string> encodingMap;
	string charBitPattern = "";
	// Build encodingMap to get instant to new char encoding. For efficiency reason better do not look each time Huffman tree for char code
	buildEncodingMap(encodingTree, "", encodingMap);
	foreach (ext_char ch in encodingMap){
		cout << char(ch) << " -- "<< encodingMap.get(ch)<<endl;	
	}
	
	while ((ch = infile.get()) != EOF){
		//cout << ch <<endl;
		charBitPattern = encodingMap.get(ch); //get char encoding
		//cout << "pass" <<endl;
		for( int i = 0; i < charBitPattern.length(); i++ ){
			//cout << "bit: " << charBitPattern.substr(i, 1) <<endl;	// test what is writing down to the stream
			outfile.writeBit(stringToInteger(charBitPattern.substr(i, 1))); //write by bit to outstream
		}
	}
	
	charBitPattern = encodingMap.get(PSEUDO_EOF);
	for( int i = 0; i < charBitPattern.length(); i++ ){
		//cout << "bit: " << charBitPattern.substr(i, 1) <<endl;	// test what is writing down to the stream
		outfile.writeBit(stringToInteger(charBitPattern.substr(i, 1))); //write by bit to outstream
	}
}

/* Function: decodeFile
 * Usage: decodeFile(encodedFile, encodingTree, resultFile);
 * --------------------------------------------------------
 * Decodes a file that has previously been encoded using the
 * encodeFile function.  You can assume the following:
 *
 *   - The encoding table has already been read from the input
 *     file, and the encoding tree parameter was constructed from
 *     this encoding table.
 *
 *   - The output file is open and ready for writing.
 */
void decodeFile(ibstream& infile, Node* encodingTree, ostream& file) {
	// TODO: Implement this!
	int bit;
	string str = "";
	Node* search = new Node;
	search = encodingTree; // start search from tree root
	while(true){
		bit = infile.readBit();
		if(bit > 0) search = search->one;
		else search = search->zero;

		//cout << "read bit"<<endl;
		if(search->character == PSEUDO_EOF){
			//cout << "break" <<endl;
			break; // stop reading bit stream
		}
		if(search->character != NOT_A_CHAR){
			//cout << "CHAR FOUND: " << char(search->character) <<endl;
			str.append(integerToString(search->character));
			file.put(search->character);
			search = encodingTree; // set search back to tree root
		}
	}
	//cout << "decode: " << str <<endl;
	delete search;
}

/* Function: writeFileHeader
 * Usage: writeFileHeader(output, frequencies);
 * --------------------------------------------------------
 * Writes a table to the front of the specified output file
 * that contains information about the frequencies of all of
 * the letters in the input text.  This information can then
 * be used to decompress input files once they've been
 * compressed.
 *
 * This function is provided for you.  You are free to modify
 * it if you see fit, but if you do you must also update the
 * readFileHeader function defined below this one so that it
 * can properly read the data back.
 */
void writeFileHeader(obstream& outfile, Map<ext_char, int>& frequencies) {
	/* The format we will use is the following:
	 *
	 * First number: Total number of characters whose frequency is being
	 *               encoded.
	 * An appropriate number of pairs of the form [char][frequency][space],
	 * encoding the number of occurrences.
	 *
	 * No information about PSEUDO_EOF is written, since the frequency is
	 * always 1.
	 */
	 
	/* Verify that we have PSEUDO_EOF somewhere in this mapping. */
	if (!frequencies.containsKey(PSEUDO_EOF)) {
		error("No PSEUDO_EOF defined.");
	}
	
	/* Write how many encodings we're going to have.  Note the space after
	 * this number to ensure that we can read it back correctly.
	 */
	outfile << frequencies.size() - 1 << ' ';
	
	/* Now, write the letter/frequency pairs. */
	foreach (ext_char ch in frequencies) {
		/* Skip PSEUDO_EOF if we see it. */
		if (ch == PSEUDO_EOF) continue;
		
		/* Write out the letter and its frequency. */
		outfile << char(ch) << frequencies[ch] << ' ';
	}
}

/* Function: readFileHeader
 * Usage: Map<ext_char, int> freq = writeFileHeader(input);
 * --------------------------------------------------------
 * Reads a table to the front of the specified input file
 * that contains information about the frequencies of all of
 * the letters in the input text.  This information can then
 * be used to reconstruct the encoding tree for that file.
 *
 * This function is provided for you.  You are free to modify
 * it if you see fit, but if you do you must also update the
 * writeFileHeader function defined before this one so that it
 * can properly write the data.
 */
Map<ext_char, int> readFileHeader(ibstream& infile) {
	/* This function inverts the mapping we wrote out in the
	 * writeFileHeader function before.  If you make any
	 * changes to that function, be sure to change this one
	 * too!
	 */
	Map<ext_char, int> result;
	
	/* Read how many values we're going to read in. */
	int numValues;
	infile >> numValues;
	
	/* Skip trailing whitespace. */
	infile.get();
	
	/* Read those values in. */
	for (int i = 0; i < numValues; i++) {
		/* Get the character we're going to read. */
		ext_char ch = infile.get();
		
		/* Get the frequency. */
		int frequency;
		infile >> frequency;
		
		/* Skip the space character. */
		infile.get();
		
		/* Add this to the encoding table. */
		result[ch] = frequency;
	}
	
	/* Add in 1 for PSEUDO_EOF. */
	result[PSEUDO_EOF] = 1;
	return result;
}

/* Function: compress
 * Usage: compress(infile, outfile);
 * --------------------------------------------------------
 * Main entry point for the Huffman compressor.  Compresses
 * the file whose contents are specified by the input
 * ibstream, then writes the result to outfile.  Your final
 * task in this assignment will be to combine all of the
 * previous functions together to implement this function,
 * which should not require much logic of its own and should
 * primarily be glue code.
 */
void compress(ibstream& infile, obstream& outfile) {
	Map<ext_char, int> freq =  getFrequencyTable(infile);
	infile.rewind();
	Node* encodingTree = buildEncodingTree(freq);
	writeFileHeader(outfile, freq);
	encodeFile(infile, encodingTree, outfile);
	freeTree(encodingTree);
}

/* Function: decompress
 * Usage: decompress(infile, outfile);
 * --------------------------------------------------------
 * Main entry point for the Huffman decompressor.
 * Decompresses the file whose contents are specified by the
 * input ibstream, then writes the decompressed version of
 * the file to the stream specified by outfile.  Your final
 * task in this assignment will be to combine all of the
 * previous functions together to implement this function,
 * which should not require much logic of its own and should
 * primarily be glue code.
 */
void decompress(ibstream& infile, ostream& outfile) {
	Map<ext_char, int> freq = readFileHeader(infile);
	Node* encodingTree = buildEncodingTree(freq);
	decodeFile(infile, encodingTree, outfile);
	//freeTree(encodingTree);

}

