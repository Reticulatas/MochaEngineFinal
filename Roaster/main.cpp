#include "FBXExporter.h"
#include "FModBankExporter.h"
#include "HACDfiles/HACD.h"
#include "HACDfiles/Helper.h"

bool CreateConvexHull(BoostPath& filePath, FBXExporter& fbxExporter)
{

		HACD::HACD_API *gHACD = HACD::createHACD_API();
		if (gHACD)
		{
			// for SetPoints
			const std::vector<Vector3>&verts = fbxExporter.GetUniqueVertices();
			if (verts.size() == 0)
				return false;

			unsigned mVertexCount = verts.size();

			// remember to delete it
			float *mVertices = new float[mVertexCount * 3];

			// move verts from file to float *
			for (unsigned i = 0; i < mVertexCount; ++i)
			{
				for (unsigned j = 0; j < 3; ++j)
				{
					if (j == 0)
						mVertices[i * 3 + j] = verts[i].x;
					if (j == 1)
						mVertices[i * 3 + j] = verts[i].y;
					if (j == 2)
						mVertices[i * 3 + j] = verts[i].z;
				}
			}


			// for SetTriangles
			const std::vector<unsigned>&indices = fbxExporter.GetUniqueIndices();
			if (indices.size() == 0)
				return false;

			unsigned mTriCount = (unsigned)indices.size() / 3;

			// remember to delete it
			int *mIndices = new int[mTriCount * 3 * sizeof(unsigned)];

			// move verts from file to float *
			for (unsigned i = 0; i < mTriCount * 3; ++i)
			{
				mIndices[i] = indices[i];
			}
			// setup parameter
			HACD::HACD_API::Desc desc;

			// setup vertex, faces
			desc.mVertices = mVertices;
			desc.mVertexCount = mVertexCount;
			desc.mIndices = (hacd::HaU32 *)mIndices;
			desc.mTriangleCount = mTriCount;

			// Chris Test Level
			//desc.mMergeHulls = true;
			//desc.mMergeHullCount = 50;
			//desc.mConcavity = 82;
			//desc.mConnectDistance = 10;

			if (desc.mTriangleCount)
			{
				hacd::HaU32 hullCount = gHACD->performHACD(desc);
				std::string hullPath("Assets\\Hulls\\");
				hullPath.append(filePath.filename().string());
				BoostPath filePath2(hullPath);
				if (!gHACD->exportScene(hullCount, filePath2))
				{
					return false;
				}
				if (hullCount != 0)
				{
					saveBin(gHACD);
					printf("Produced %d output convex hulls.\r\n", hullCount);
					FILE *fph = fopen("ConvexDecomposition.obj", "wb");

					bool generateConstraints = false;

					if (fph)
					{
						//fprintf(fph, "# Input mesh '%s' produced %d convex hulls.\r\n", wavefront, hullCount);
						hacd::HaU32 *baseVertex = new hacd::HaU32[hullCount];
						hacd::HaU32 vertexCount = 0;
						for (hacd::HaU32 i = 0; i < hullCount; i++)
						{
							const HACD::HACD_API::Hull *hull = gHACD->getHull(i);
							if (hull)
							{
								baseVertex[i] = vertexCount;
								fprintf(fph, "## Hull %d has %d vertex.\r\n", i + 1, hull->mVertexCount);
								for (hacd::HaU32 i = 0; i < hull->mVertexCount; i++)
								{
									const hacd::HaF32 *p = &hull->mVertices[i * 3];
									fprintf(fph, "v %0.9f %0.9f %0.9f\r\n", p[0], p[1], p[2]);
								}
								vertexCount += hull->mVertexCount;
							}
						}
						for (hacd::HaU32 i = 0; i < hullCount; i++)
						{
							const HACD::HACD_API::Hull *hull = gHACD->getHull(i);
							if (hull)
							{
								hacd::HaU32 startVertex = baseVertex[i];
								fprintf(fph, "# Convex Hull %d contains %d triangles and %d vertex.  Starting vertex index is: %d It has a volume of: %0.9f\r\n", i + 1, hull->mTriangleCount, hull->mVertexCount, startVertex);
								for (hacd::HaU32 j = 0; j < hull->mTriangleCount; j++)
								{
									hacd::HaU32 i1 = hull->mIndices[j * 3 + 0] + startVertex + 1;
									hacd::HaU32 i2 = hull->mIndices[j * 3 + 1] + startVertex + 1;
									hacd::HaU32 i3 = hull->mIndices[j * 3 + 2] + startVertex + 1;
									fprintf(fph, "f %d %d %d\r\n", i1, i2, i3);
								}
							}
						}
					}
					else
					{
						printf("Failed to open output file.\r\n");
					}
					if (generateConstraints && hullCount > 1)
					{
						printf("Auto generating constraints between the convex hulls.\r\n");
						hacd::HaU32 constraintCount = gHACD->generateConstraints();
						printf("Generated %d constraints.\r\n", constraintCount);
						for (hacd::HaU32 i = 0; i < constraintCount; i++)
						{
							const HACD::HACD_API::Constraint *c = gHACD->getConstraint(i);
							if (c)
							{
								printf("Constraint%d : From Hull %d to Hull %d at location(%0.4f,%0.4f,%0.4f)\r\n", i + 1,
									c->mFrom,
									c->mTo,
									c->mIntersect[0],
									c->mIntersect[1],
									c->mIntersect[2]);
							}
						}
					}
				}
			}
			delete[] mVertices;
			delete[] mIndices;
		}
}

int main(int argc, char** argv)
{
  // Do meta file checking
  FBXExporter fbxExporter;
  std::string HullName("Maze.bean");
  // Initialize exporter
  bool result = fbxExporter.Initialize();

  if (!result)
  {
    std::cout << "Failed to initialize ROASTER!" << std::endl;
    return 0;
  }

  for (boost::filesystem::recursive_directory_iterator end, dir("Assets/"), dirHull("Assets/Hulls/"); dir != end; ++dir)
  {
    if (boost::filesystem::is_regular_file(*dir))
    {
      std::string extCheck = dir->path().filename().extension().string();
      std::transform(extCheck.begin(), extCheck.end(), extCheck.begin(), ::tolower);

      if (extCheck == FbxExt)
      {
        //this will create the available asset if it does not exist
        BoostPath filePath = dir->path().string();            
        BoostPath metaPath, beanPath = filePath, animPath = filePath;  
        beanPath.replace_extension(BeanExt + MetaExt);
        animPath.replace_extension(AnimExt + MetaExt);

        metaPath = beanPath;
        if (boost::filesystem::exists(animPath))
          metaPath = animPath;

        // Check time stamp
        FILETIME writeTime = GetTimeStamp(filePath);

        // Metafile exists
        if (boost::filesystem::exists(metaPath))
        {
          FILETIME metaTime = GetMetaTimeStamp(metaPath);

          // Compare timings
          if (CompareFileTime(&metaTime, &writeTime) < 0)
          {
            fbxExporter.LoadFBX(filePath.string());

            result = fbxExporter.ExportScene(filePath);
            if (result)
              std::cout << "Successfully exported: " << filePath.filename().string() << std::endl << std::endl;

			if (filePath.filename().string() == HullName) // if it's a level mesh
			{
				result = CreateConvexHull(filePath, fbxExporter);
				if (!result)
					std::cout << "Failed to export Hull" << std::endl;
			}

			fbxExporter.GetUniqueVertices().clear();
			fbxExporter.GetUniqueIndices().clear();
			fbxExporter.GetUniqueTriangle().clear();
            // Let ModelLoader handle time stamp

            //filePath += MetaExt;
            //// Write time stamp to meta file
            //WriteTimeStamp(filePath, writeTime);
          }          
        }

        // Metafile doesnt exist, create one
        else
        {
          fbxExporter.LoadFBX(filePath.string());

          result = fbxExporter.ExportScene(filePath);
          if (result)
            std::cout << "Successfully exported: " << filePath.filename().string() << std::endl << std::endl;

		  if (filePath.filename().string() == HullName) // if it's a level mesh
		  {
			  result = CreateConvexHull(filePath, fbxExporter);
			  if (!result)
				  std::cout << "Failed to export Hull" << std::endl;
		  }

		  fbxExporter.GetUniqueVertices().clear();
		  fbxExporter.GetUniqueIndices().clear();
		  fbxExporter.GetUniqueTriangle().clear();

          BoostPath animString = filePath.string();

          filePath += MetaExt;
          // Write time stamp to meta file
          WriteTimeStamp(filePath, writeTime);

          //if (animString.extension().string() == AnimExt)
          //  WriteAnimStamp(animString);            
        }
      }
      else if(dir->path().filename() == FModGUIDFile)
      {
          BoostPath filePath = dir->path().branch_path().string();
          FModBankExporter::ExportEventFiles(filePath.string());
      }
    }
  }

  return 0;
}