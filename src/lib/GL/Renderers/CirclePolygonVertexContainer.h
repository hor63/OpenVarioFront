/*
 * CirclePolygonVertexContainer.h
 *
 *  Created on: Apr 22, 2025
 *      Author: hor
 */

#ifndef RENDERERS_CIRCLEPOLYGONVERTEXCONTAINER_H_
#define RENDERERS_CIRCLEPOLYGONVERTEXCONTAINER_H_

#include <array>
#include <map>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES2/gl2platform.h>

#include "VecMat.h"

namespace OevGLES {

class GLFramework;

/** \brief
 *
 */
class CirclePolygonVertexContainer final {
public:

	// Only you can create me.
	friend class GLFramework;

	struct CirclePolygonVertexStruct {
		GLfloat position[4];
		/** \brief Normal to the circle circumference
		 *
		 * This is *not* the normal of the triangle, but the normal of the
		 * circumference circle of the polygon at the vertex position
		*/
		GLfloat normal[4];
		/** \brief Indicator if the vertex is on the primary or secondary circle
		 *
		 * A value 0.0 indicates the vertex is part of the primary circle.\n
		 * A value 1.0 indicates the vertex is part of the secondary circle.
		 * The primary and secondary circle are connected by a triangle mesh.
		 *
		 * What the primary and secondary circle is depends on the use:
		 *   In case of a ring (annulus) the primary circle is the outer circle,
		 *   and the secondary circle is the inner circle. A full circle is just
		 *   a degenerated ring with a secondary radius of 0\n
		 *   In case of a cylinder the primary circle is on the z=0 plane, the
		 *   secondary circle shares the same diameter and x-y center but is
		 *   offset in z-direction.
		 *
		 * How the secondary circle is formed is determined by a transformation
		 * matrix uniform provided to the program.
		 *
		*/
		GLfloat isSecondaryCircle;

		CirclePolygonVertexStruct() :
			position {0.0f,0.0f,0.0f,1.0f},
			// x and y are being calculated in the constructor of CirclePolygonVertexContainer.
			// Value 1.0 for z is a dummy which will be adjusted by the z-factor
			// by the internal model matrix for the normal.
			// But to apply a factor I need a value != 0 initially
			normal {0.0f,0.0f,1.0f,0.0f},
			isSecondaryCircle{0.0f}
		{ }
	};

	struct CircleVertexArrayStruct {
		/** \brief Number of vertexes forming a full primary and secondary circle
		 *
		 * The number of vertexes is numSegments*2+4. This is due to the fact
		 * that the vertexes for the primary and secondary circle are intermittent,
		 * and the circle must be closed at the end,
		 * and the first 2 vertexes are being used as circle center for the 
		 * full circle class \ref CircleFilledRenderer (hence the "+2" term).
		*/
		GLsizei numVertexes;
		
		/** \brief stride within \ref maxSegmentVertexArray for this polygon
		 *
		 * This object usually represents a polygon with less segments as \ref maxNumSegments
		 * and less vertexes than in \ref maxSegmentVertexArray.
		 * 
		 * This factor here gives you the stepping within \ref maxSegmentVertexArray
		 * to access the first vertex for segment \#n in your polygon within 
		 * \ref maxSegmentVertexArray.
		 * 
		 */
		std::size_t vertexStrideInMaxVertexArrayPerSegment;
		
		/** \brief
		 * The radius of the circle where the deviation of the polygon from
		 * the ideal circle becomes larger than \ref maxDeviationPixels.
		 */
		GLfloat maxRadius;
		std::size_t numSegments;
		AngleRad angleIncrement;
		/** \brief Handle to the GL ES vertex buffer
		 *
		 * The vertex buffer is created only on demand. The handle is initialized
		 * to 0 to indicate that the vertex buffer must still be created.
		 *
		 * Please note that I am usually not storing the content of the buffer in
		 * the program (i.e. on the client side) except from the vertex array for
		 * \ref maxNumSegments segments. This is the largest array. All other
		 * arrays can be copied from the largest array, leaving out not needed
		 * vertexes. Therefore I need to perform the complex calculations for
		 * the vertexes only once.
		*/
		GLuint vertexBufferHandle;
		GLuint vertexArrayHandle;

		CircleVertexArrayStruct() = delete;

		CircleVertexArrayStruct(std::size_t numSegments);

		CircleVertexArrayStruct(CircleVertexArrayStruct const& source)
			:numSegments{source.numSegments},
			 numVertexes{source.numVertexes},
			 vertexStrideInMaxVertexArrayPerSegment{source.vertexStrideInMaxVertexArrayPerSegment},
			 angleIncrement{source.angleIncrement},
			 maxRadius{source.maxRadius},
			 vertexBufferHandle{0},
			 vertexArrayHandle{0}
		{}

		CircleVertexArrayStruct(CircleVertexArrayStruct&& source)
			:numSegments{source.numSegments},
			 numVertexes{source.numVertexes},
			 vertexStrideInMaxVertexArrayPerSegment{source.vertexStrideInMaxVertexArrayPerSegment},
			 angleIncrement{source.angleIncrement},
			 maxRadius{source.maxRadius},
			 vertexBufferHandle{source.vertexBufferHandle},
			 vertexArrayHandle{source.vertexArrayHandle}
		{
			source.vertexBufferHandle = 0;
			source.vertexArrayHandle = 0;
		}

		~CircleVertexArrayStruct();

		CircleVertexArrayStruct& operator = (CircleVertexArrayStruct&& source);
		
		CircleVertexArrayStruct& operator = (CircleVertexArrayStruct const& source);
	};

	/** \brief Maximum allowed deviation from the ideal circular form
	 *
	 * Circles are being composed polygons.
	 * The number of segments and the radius of the polygon determine the
	 * max. deviation from the ideal circle. \n
	 * Conversely a given number of polygon segments determines the radius
	 * of the polygon to not exceed the max. deviation from the circular form.
	 */
	static constexpr double maxDeviationPixels = 0.5;

	/** \brief Maximum number of segments of a polygon
	 *
	 * For reference:
	 * 	- With 64 segments a deviation of 0.5 pixel occurs at a radius of 415 pixel
	 * 	- With 128 segments a deviation of 1 pixel occurs at a radius of 1660 pixel
	 *
	 * 	Therefore a maximum of 256 segments is plenty enough.
	 */
	static constexpr std::size_t maxNumSegments = 256;

	~CirclePolygonVertexContainer();

	/** \brief Create and cache a vertex buffer for a circular object with a given
	 * radius.
	 *
	 * *Note*: The returned reference is owned by this object, and must not be used
	 * after this \ref CirclePolygonVertexContainer is deleted.
	 * This is itself usually owned by the \ref GLFramework object of the program.
	 *
	 * The returned structure is cached internally. The returned \ref
	 * CircleVertexArrayStruct is supposed to be used multiple times.
	 * However, the vertex buffer is created on demand only. I.e. the vertex buffer
	 * is not created before a vertex buffer with the requested number of segments
	 * is being requested.
	 *
	 * \param radius Intended radius of the circular object to draw.
	 * 		It is assumed that the outer radius of the object is passed.
	 * \return Reference to a \ref CircleVertexArrayStruct structure with a
	 * vertex buffer handle, and further description of the vertex buffer.
	 * The number of polygon segments returned vertex buffer is always always a
	 * power of 2 plus one segment to match the first segment to close the circle.
	 */
	CircleVertexArrayStruct const &createVertexArrayStruct (GLfloat radius);

	typedef std::array<CirclePolygonVertexStruct,maxNumSegments*2 + 4> MaxSegmentVertexArrayType;

	/** \brief Provide direct access to the largest possible vertex array on the 
	 *   client side
	 */
	const MaxSegmentVertexArrayType getMaxSegmentVertexArray() const {
		return maxSegmentVertexArray;
	}

private:

	/** \brief An array with the maximum number of segments
	 *
	 * All other vertex arrays can be constructed from the values in this array.
	 *
	 * The array contains \ref maxNumSegments*2 + 4 elements because each segment
	 * has per angle one item on the primary, and one element on the secondary
	 * circle.
	 * At the beginning there are two elements to be used as center of a full circle.
	 * At the end 2 elements are added to close the circle from
	 * \ref maxNumSegments -1 to \ref maxNumSegments.
	 *
	*/
	MaxSegmentVertexArrayType maxSegmentVertexArray;

	/** \brief Contains a map of all possible circle segmentations
	 *
	 * The minimum number of segments is 4 (hardly a circle :) ); maximum number
	 * of segments is \ref maxNumSegments.
	 *
	 * The key is the maximum radius (not diameter!) in pixels pixels where the
	 * deviation from the ideal circle becomes > \ref maxDeviationPixels.
	*/
	using  CircleVertexArrayMapType = std::map<GLfloat,CircleVertexArrayStruct>;
	CircleVertexArrayMapType circleVertexArrayMap;

	/// Only friend \ref GLFramework can create me.
	CirclePolygonVertexContainer();

	void createVertexBuffer (CircleVertexArrayStruct& vertArrayStruct);

};

} /* namespace OevGLES */

#endif /* RENDERERS_CIRCLEPOLYGONVERTEXCONTAINER_H_ */
