using System;
using System.Collections.Generic;
using MochaEditor;
using MochaEditorControls;
using System.Collections.ObjectModel;

public class GameObjectSelectionManager
{
    public static void RequestSelect(GameObject gobj)
    {
        foreach (GameObject priorSelectedObj in EngineManagerViewModel.instance.SelectedGameObjects)
            priorSelectedObj.IsSelected = false;
        gobj.IsSelected = true;
        EngineManagerViewModel.instance.SelectedGameObjects.Replace(gobj);
        EngineManagerViewModel.instance.ForceSelectedGameObjectRefresh();
        HierarchyControlSelectionManager.HierarchyControlStateManager.GObjSelectRefresh(gobj);
    }

    public static void RequestUnSelect(GameObject gobj)
    {
        gobj.IsSelected = false;
        EngineManagerViewModel.instance.SelectedGameObjects.Remove(gobj);
        EngineManagerViewModel.instance.ForceSelectedGameObjectRefresh();
        HierarchyControlSelectionManager.HierarchyControlStateManager.GObjAdditiveSelectRefresh(gobj);
    }

    public static void RequestAdditiveSelect(GameObject gobj)
    {
        gobj.IsSelected = true;
        EngineManagerViewModel.instance.SelectedGameObjects.Add(gobj);
        EngineManagerViewModel.instance.ForceSelectedGameObjectRefresh();
        HierarchyControlSelectionManager.HierarchyControlStateManager.GObjAdditiveSelectRefresh(gobj);
    }

    public static void RequestRangeSelect(GameObject gobj, ObservableCollection<GameObject> tierList)
    {
        if (EngineManagerViewModel.instance.SelectedGameObjects.Count == 0)
            return;
        int a = tierList.IndexOf(EngineManagerViewModel.instance.SelectedGameObjects[EngineManagerViewModel.instance.SelectedGameObjects.Count-1]);
        int b = tierList.IndexOf(gobj);

        int end_index = Math.Max(a, b);
        int start_index = Math.Min(a, b);

        if (end_index == start_index || end_index == -1 || start_index == -1)
            return;

        for (int i = start_index; i <= end_index; ++i)
            RequestAdditiveSelect(tierList[i]);
    }
}

public class HierarchyControlSelectionManager
{

    public static void RequestSelect(HierarchyControl hc, GameObject gobj)
    {
        GameObjectSelectionManager.RequestSelect(gobj);
        HierarchyControlStateManager.Select(hc, true);
        hc.BringIntoView();
    }

    public static bool RequestSelectDeselect(HierarchyControl hc, GameObject gobj)
    {
        if (EngineManagerViewModel.instance.SelectedGameObjects.Contains(gobj))
        {
            RequestUnSelect(hc, gobj);
            return false;
        }
        RequestSelect(hc, gobj);
        return true;
    }

    public static bool RequestAdditiveSelectDeselect(HierarchyControl hc, GameObject gobj)
    {
        if (EngineManagerViewModel.instance.SelectedGameObjects.Contains(gobj))
        {
            RequestUnSelect(hc, gobj);
            return false;
        }
        RequestAdditiveSelect(hc, gobj);
        return true;
    }

    public static void RequestAdditiveSelect(HierarchyControl hc, GameObject gobj)
    {
        GameObjectSelectionManager.RequestAdditiveSelect(gobj);
        HierarchyControlStateManager.Select(hc, false);
    }

    public static void RequestUnSelect(HierarchyControl hc, GameObject gobj)
    {
        GameObjectSelectionManager.RequestUnSelect(gobj);
        HierarchyControlStateManager.UnSelect(hc);
    }
    public static void RequestRangeSelect(MochaEditorControls.HierarchyControl hierarchyControl, GameObject gameObject)
    {
        HierarchyControl source_parent = hierarchyControl.ParentHierarchyControl;
        if (source_parent == null)
            return;

        GameObjectSelectionManager.RequestRangeSelect(gameObject, source_parent.Children);
    }

    public class HierarchyControlStateManager
    {

        /*
         * NO EXTERNAL MODIFICATIONS
         * NO MODIFICATIONS OF ENGINEMANAGERVIEWMODEL - USE REQUESTS FOR THAT 
         */
        static Dictionary<GameObject, HierarchyControl> gObjToControlLookup = new Dictionary<GameObject, HierarchyControl>();
        static HashSet<HierarchyControl> selectionInfo = new HashSet<HierarchyControl>();
        static HashSet<HierarchyControl> currentlySelected = new HashSet<HierarchyControl>();

        public static void RequestRemoval(GameObject g)
        {
            if (!gObjToControlLookup.ContainsKey(g))
                return;
            HierarchyControl hc = gObjToControlLookup[g];
            if (hc.ParentHierarchyControl != null)
                hc.ParentHierarchyControl.IntermediateSource.Remove(g);
            DeRegisterHierarchyControl(hc);
        }

        static void Set(HierarchyControl hc, bool isselected)
        {
            hc.IsSelected = isselected;
            if (isselected)
                currentlySelected.Add(hc);
            else
                currentlySelected.Remove(hc);
        }

        public static void RegisterHierarchyControl(HierarchyControl hc)
        {
            selectionInfo.Add(hc);
            try
            {
                if (!hc.IsFolder)
                    gObjToControlLookup.Add(hc.myGameObject, hc);
            }catch(ArgumentException e)
            {
                Logger.Log("Hierarchy Control Error: " + e.Message + " Loaded/Unloaded Faulty - Tell Fuller.");
            }
        }

        public static void DeRegisterHierarchyControl(HierarchyControl hc)
        {
            selectionInfo.Remove(hc);
            if (!hc.IsFolder)
                gObjToControlLookup.Remove(hc.myGameObject);
        }

        //Sensitive entry point for game object changes
        public static void GObjSelectRefresh(GameObject toSelect)
        {
            foreach (GameObject gobj in gObjToControlLookup.Keys)
            {
                try
                {
                    if (gobj.IsSelected)
                        Select(gObjToControlLookup[gobj], false);
                    else
                        UnSelect(gObjToControlLookup[gobj]);
                }
                catch (KeyNotFoundException e)
                {
                    Logger.Log("Cannot Select/Unselect - " + e.Message);
                }
            }

            GObjAdditiveSelectRefresh(toSelect);
        }
        public static void GObjAdditiveSelectRefresh(GameObject gobj)
        {
            if (!gObjToControlLookup.ContainsKey(gobj))
            {
                Logger.Log("Unregistered HierarchyControl attempted to be accessed");
                return;
            }
            if (gobj.IsSelected)
                Select(gObjToControlLookup[gobj], false);
            else
                UnSelect(gObjToControlLookup[gobj]);
        }

        public static void Select(HierarchyControl hc, bool deselectOthers = true)
        {
            if (deselectOthers)
            {
                foreach (var hier in currentlySelected)
                    hier.IsSelected = false;
                currentlySelected.Clear();
            }
            Set(hc, true);
        }

        public static void UnSelect(HierarchyControl hc)
        {
            Set(hc, false);
        }

        public static void SelectAll()
        {
            foreach (var hier in selectionInfo)
                Set(hier, true);
        }
        public static void DeSelectAll()
        {
            foreach (var hier in selectionInfo)
                Set(hier, false);
        }
    }


}

