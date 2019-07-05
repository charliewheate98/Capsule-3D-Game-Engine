/*
* Marco definitions
*/
#ifndef __QUERY_H__
#define __QUERY_H__

/*
* Includes
*/
#include <iostream>
#include <glew.h>

/*
* Query class: this class handles and generates a query for use in 
* occlusion culling and lens flares (basically checking for occluders)
* with queries we are able to check for the amount of samples passed from the zbuffer (depth buffer)
* therefore allowing us to check for visiblity of objects in the scene
*/
class Query
{
private:
	/*
	* Variables
	*/
	GLuint id;
	GLuint type;
	GLint  samples_passed;
	GLint  avalible;
	bool _in_use;
public:
	/*
	* Constructer
	*/
	inline Query(GLuint query_type, GLsizei num_queries = 1)
	{
		/*
		* Initialise identifiers
		*/
		type = query_type;
		_in_use = false;

		/*
		* Generate the query (num queries: 1 by default)
		*/
		glGenQueries(num_queries, &id);
	}

	/*
	* Destructer
	*/
	inline ~Query()
	{
		glDeleteQueries(1, &id);
	}

	/*
	* Get the resulting samples from the zbuffer
	*/
	inline void getResult()
	{
		glGetQueryObjectiv(id, GL_QUERY_RESULT, &samples_passed);
	}

	/*
	* Check if the result is avalible
	*/
	inline void checkResult()
	{
		glGetQueryObjectiv(id, GL_QUERY_RESULT_AVAILABLE, &avalible);
	}

	/*
	* Check if the query is currently being used
	*/
	inline bool QueryInUse() { return _in_use; }

	/*
	* Get the amount of samples passed(visible)
	*/
	inline GLint getSamplesPassed() { return samples_passed; }

	/*
	* Get avalibility of the result (is it ready to use yet?)
	*/
	inline GLint getAvalibility() { return avalible; }

	/*
	* Start the query (is now in use)
	*/
	inline void start()
	{
		glBeginQuery(type, id);
		_in_use = true;
	}

	/*
	* End the query (no longer in use)
	*/
	inline void end()
	{
		glEndQuery(type);
		_in_use = false;
	}
};

// End of class
#endif