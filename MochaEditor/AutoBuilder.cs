using System;
using MochaInterface;
using Microsoft.Xna.Framework;
using MochaEditor;

public class AutoBuilder
{
    #region MATH
    struct Plane {
        public Vector3 pos;
        public Vector3 Normal;
    }
    struct Ray3
    {
       public Vector3 pos, dir;
    };

    bool Ray3DToPlane(Ray3 ray, Plane plane, ref Vector3 result)
    {
        float parallelResult = Vector3.Dot(ray.dir, plane.Normal);

        if (Math.Abs(parallelResult) > 0)
        {
            float d = Vector3.Dot((plane.pos - ray.pos), plane.Normal);
            d /= parallelResult;
            result = ray.pos + d * ray.dir;
            return true;
        }
        return false;
    }
    #endregion

    Vector3 startPoint;     //starting point of the operation
    Vector3 boundingBox = new Vector3(0.5f,0.5f,0.5f);
    Vector3 boundingBoxOffset;
    bool operationStarted = false;  //are we currently dragging

    public void Update()
    {
        //only works when keyboard b is held
        if (Input.GetHeld(0, "B") == 0.0f)
        {
            operationStarted = false;
            return;
        }

        Plane plane;
		plane.Normal = Vector3.Up;
        plane.pos = Vector3.Zero;

		//intersect with plane
		Vector3 mouse_ray = EditorSubsystem.getInstance().EditorCamera().ScreenToWorldPosition(Input.GetMouseX(true), Input.GetMouseY(true));
		Ray3 mouse_ray3;
		mouse_ray3.dir = mouse_ray;
        mouse_ray3.dir.Normalize();
		mouse_ray3.pos = EditorSubsystem.getInstance().EditorCamera().gameObject.transform.GetGlobalPosition();

        Vector3 intersect_pt = Vector3.Zero;
        if (Ray3DToPlane(mouse_ray3, plane, ref intersect_pt))
        {
            //snap to nearest 1
            intersect_pt.X = (float)Math.Round(intersect_pt.X);
            intersect_pt.Y = 0;
            intersect_pt.Z = (float)Math.Round(intersect_pt.Z);

            //draw debug box to show end point
            Renderer.getInstance().DrawBoxPerma(intersect_pt - boundingBoxOffset, boundingBox, new Color(0, 0, 1.0f, 1.0f));

            //draw line to show that we are fit to an axis
            if (operationStarted)
            {
                if (startPoint.X == intersect_pt.X || startPoint.Z == intersect_pt.Z)
                    Renderer.getInstance().DrawLinePerma(startPoint, intersect_pt, new Color(0, 1.0f, 0,1.0f));
            }

            if (Input.GetTriggered(0, "MouseLClick") > 0.0f)
            {
                if (operationStarted)
                {
                    //end operation
                    Vector3 end_point = intersect_pt;

                    FitMeshToPoints(startPoint, end_point);

                    operationStarted = false;
                }
                else
                {
                    startPoint = intersect_pt;

                    if (EngineManagerViewModel.instance.SelectedGameObjects.Count > 0)
                    {
                        GameObject g = EngineManagerViewModel.instance.SelectedGameObjects[0];
                        AABB gAABB = g.GetComponent<CMeshRenderer>().GetMesh().GetAABB();
                        boundingBox = ((Vector3)gAABB.maxPoint - (Vector3)gAABB.minPoint) * 0.5f;
                        boundingBox.X *= g.transform.scale.x;
                        Vector3 AABBCenter = ((Vector3)gAABB.maxPoint + (Vector3)gAABB.minPoint) * 0.5f;
                        boundingBoxOffset = (AABBCenter - gAABB.minPoint);
                        boundingBoxOffset.Y = 0;
                    }
                    else
                        boundingBox = new Vector3(0.5f, 0.5f, 0.5f);
                    operationStarted = true;
                }
            }
        }
    }

    /// Will only change the Z-scale and Y-rotation to fit mesh between two points
    void FitMeshToPoints(Vector3 start, Vector3 end)
    {
        if (EngineManagerViewModel.instance.SelectedGameObjects.Count == 0)
        {
            Logger.Log("Select a game object first to use auto-builder.");
            return;
        }

        GameObject g = EngineManagerViewModel.instance.SelectedGameObjects[0];

        if (g.GetComponent<CMeshRenderer>() == null)
        {
            Logger.Log("Auto Builder requires Renderer to be attached to first selected object.");
            return;
        }

        //grab mesh model space aabb
        AABB gAABB = g.GetComponent<CMeshRenderer>().GetMesh().GetAABB();

        //rot
        g.transform.SetPosition(start);
        g.transform.LookAt(end);

        //pos and scale
        float offZ = (gAABB.maxPoint.z - gAABB.minPoint.z);
        float pz = g.transform.position.z;

        float spanL = (end - start).Length() / offZ;
        //account for offset
        spanL += 1;

        //float pzPrime = (offZ * (spanL*0.5f));
        float pzPrime = (end - start).Length() * .5f / offZ;
        float szPrime = spanL;

        Vector3 pPrime = g.transform.position + pzPrime * (Vector3)g.transform.GetForwardVector();

        pPrime -= boundingBoxOffset;

        g.transform.SetPosition(pPrime);
        g.transform.scale.z = szPrime;
    }
}
