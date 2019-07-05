#ifndef __CHUNK_H__
#define __CHUNK_H__

// This will contain a chunk of geometry data
struct Chunk
{
	unsigned int _index_offset;		// The index offset for this element
	unsigned int _index_count;	// The index count for this element
	unsigned int _id;	// The chunk id for this element

	// Default constructor
	inline Chunk() = default;

	// Initial constructor
	inline Chunk(unsigned int index_offset, unsigned int index_count, unsigned int id)
	{
		_index_offset = index_offset;	// Assign data
		_index_count = index_count;		// Assign data 
		_id = id;	// Assign data
	}
};

#endif