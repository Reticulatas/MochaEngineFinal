using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Threading;
using Microsoft.Xna.Framework;
using MochaInterface;
using System.Threading;

namespace MochaEditor
{
    public class EditorSubsystemManaged
    {

        public class EditorAction
        {
            public Action a;
            public ManualResetEventSlim mres;   //possible bound reset event for blocked events
        }

        Queue<EditorAction> actionQueue = new Queue<EditorAction>();
        uint checkforChangedAssetsTimer = 0;
        uint CHECKFORCHANGEDASSETS_TIME = 100;
        float autoSaveTimer = 0;
        float _lookTimer = 0;       //double press to zoom
        Vector3 cameraVelocity = new Vector3();

        float arrowSnapPrecision = 0.5f;

        CFontRenderer mouseTooltipText;
        CFontRenderer editorStatusText;

        //sub-systems
        AutoBuilder auto_builder = new AutoBuilder();

        bool hasInit = false;
        void Init()
        {
            if (hasInit == true)
                return;

            //we assume on first update that the editor subsystem is valid
            GameObject tooltip = GameObject.From(ObjectFactory.getInstance().MakeNewGameObject("EditorMouseTooltip", EditorSubsystem.getInstance().GetEditorState(), true));
            mouseTooltipText = tooltip.RequireComponent<CFontRenderer>();
            mouseTooltipText.mPerspective = false;
            mouseTooltipText.mFontSize = 12;
            GameObject status = GameObject.From(ObjectFactory.getInstance().MakeNewGameObject("EditorStatusTooltip", EditorSubsystem.getInstance().GetEditorState(), true));
            editorStatusText = status.RequireComponent<CFontRenderer>();
            editorStatusText.mPerspective = false;
            editorStatusText.mFontSize = 25;
            editorStatusText.mFontScale = 0.5f;

            hasInit = true;

            //first time script reload when editor in progress
            MochaScriptSystem.ScriptSubsystemManaged.getInstance().Reload();
        }

        #region SUBSYSTEMHOOKS
        Controls.ModernModalWindow mmw = null;
        void ScriptSubsystemManaged_HasReloaded(object sender, EventArgs e)
        {
            Application.Current.Dispatcher.BeginInvoke(new Action(() =>
            {
                if (mmw != null)
                    mmw.Close();
                mmw = null;
            }));
        }

        void ScriptSubsystemManaged_IsReloading(object sender, EventArgs e)
        {
            Application.Current.Dispatcher.BeginInvoke(new Action(() =>
            {
                mmw = new Controls.ModernModalWindow("Compiling Scripts...");
            }));
        }

        /* Pre-script system hooks initialization */
        internal void RegisterHooks()
        {
            MochaScriptSystem.ScriptSubsystemManaged.IsReloading += ScriptSubsystemManaged_IsReloading;
            MochaScriptSystem.ScriptSubsystemManaged.HasReloaded += ScriptSubsystemManaged_HasReloaded;
        }
        #endregion

        #region EDITORQUEUE

        public void QueueAction(Action _a)
        {
            actionQueue.Enqueue(new EditorAction { a = _a, mres = null });
        }

        public void QueueActionAndBlock(Action _a)
        {
            ManualResetEventSlim _mres = new ManualResetEventSlim(false);
            actionQueue.Enqueue(new EditorAction { a = _a, mres = _mres });

            //block until we signal a return
            _mres.Wait();
        }

        public void QueueWaitFrame()
        {
            actionQueue.Enqueue(null);
        }

        #endregion

        public void Tick()
        {
            if (!hasInit)
                Init();

            while (actionQueue.Count != 0)
            {
                EditorAction a = actionQueue.Dequeue();
                //Dispatcher.CurrentDispatcher.Invoke(a, DispatcherPriority.Normal);
                if (a == null)  //wait frame
                    break;
                a.a();
                if (a.mres != null)
                {
                    a.mres.Set();
                    a.mres.Dispose();
                }
            }

            if (!EditorSubsystem.getInstance().IsTesting())
            {

                //draw grid lines
                if (EngineCommandBindings.DrawGrid && EngineCommandBindings.GridSize > 0)
                {
                    Vector3 camPos = EditorSubsystem.getInstance().EditorCamera().gameObject.transform.GetGlobalPosition();
                    float startX = camPos.X;
                    float startY = camPos.Y;
                    float startZ = camPos.Z;
                    startX = startX - (startX % EngineCommandBindings.GridSize);
                    if (startY > 0)
                        startY = startY - (startY % EngineCommandBindings.GridSize);
                    else
                        startY = startY + (startY % EngineCommandBindings.GridSize);
                    startZ = startZ - (startZ % EngineCommandBindings.GridSize);

                    for (int y = (int)startY; y <= (int)startY + EngineCommandBindings.gridExtent / 2; y += EngineCommandBindings.GridSize)
                    {
                        //for every y, draw an x,z plane
                        for (int x = (int)startX - EngineCommandBindings.gridExtent; x <= (int)startX + EngineCommandBindings.gridExtent; x += EngineCommandBindings.GridSize)
                        {
                            //draw z
                            Renderer.getInstance().DrawLinePerma(new Vector3(x, y, startZ - EngineCommandBindings.gridExtent), new Vector3(x, y, startZ + EngineCommandBindings.gridExtent),
                                new Color(0.39f, 0.60f, 0.93f, .5f));
                            for (int z = (int)startZ - EngineCommandBindings.gridExtent; z <= (int)startZ + EngineCommandBindings.gridExtent; z += EngineCommandBindings.GridSize)
                            {
                                //draw x
                                Renderer.getInstance().DrawLinePerma(new Vector3(x - EngineCommandBindings.gridExtent, y, z), new Vector3(x + EngineCommandBindings.gridExtent, y, z),
                                        new Color(.4f, .4f, 0.0f, .3f));
                            }
                        }
                    }
                }

                //save occasionally
                autoSaveTimer += FrameController.DT();
                if (autoSaveTimer > 30.0f)
                {
                    QueueAction(() =>
                    {
                        try
                        {
                            StateSerializer ss = new StateSerializer();
                            ss.DoRecoverySave();
                        }
                        catch (Exception e)
                        {
                            Logger.Log("FAILED TO SAVE RECOVERY SAVE: " + e.Message);
                        }
                    });
                    autoSaveTimer = 0;
                }

                //occasionally check for available assets
                ++checkforChangedAssetsTimer;
                if (checkforChangedAssetsTimer > CHECKFORCHANGEDASSETS_TIME)
                {
                    AssetManager.getInstance().CheckForChangedAssets();
                    checkforChangedAssetsTimer = 0;
                }

                //do standard updates
                if (Input.GetIsMouseInWindow(true) && EngineManagerViewModel.instance.EngineEmbedHasVisibility && EngineManagerViewModel.instance._isFocused)
                {
                    //run sub systems
                    auto_builder.Update();

                    //set tooltips
                    mouseTooltipText.gameObject.transform.SetPosition(Input.GetMouseX(true) - Engine.getInstance().GetGameWnd().GetWindowWidth() * 0.5f,
                       -(Input.GetMouseY(true) - Engine.getInstance().GetGameWnd().GetWindowHeight() * 0.5f), 0);
                    editorStatusText.gameObject.transform.SetPosition(-Engine.getInstance().GetGameWnd().GetWindowWidth() * 0.49f,
                       Engine.getInstance().GetGameWnd().GetWindowHeight() * 0.44f, 0);
                    editorStatusText.mText = "--- STATUS ---\\n" +
                        "Arrow Keys Snap Prec.: " + arrowSnapPrecision.ToString("0.0") +
                        "\\nFPS: " + (1.0f / FrameController.DT()).ToString("0.0") +
                        "\\n---------------";

                    CCamera editorCamera = EditorSubsystem.getInstance().EditorCamera();
                    if (Input.GetTriggered(0, "F2") == 1.0f)
                    {
                        //duplicate
                        EngineCommandBindings.CMD_DuplicateObjects();
                    }

                    //select objects naively
                        GameObject gobj = GameObject.From(PhysicEngine.getInstance().RayCast3D(editorCamera.gameObject.transform.position,
                        editorCamera.gameObject.transform.GetForwardVector(), editorCamera, Input.GetMouseX(true),
                            Input.GetMouseY(true), 10000.0f));
                    if (gobj != null && gobj.GetFlag("ArtWidget"))
                    {
                        //select widget's actual object
                        gobj = GameObject.From(EditorSubsystem.getInstance().widgetManager.GetAttachedObjFromWidgetObj(gobj));
                    }
                    if (gobj != null && !gobj.GetFlag("Widget"))
                    {
                        //show tooltip if mouse over
                        mouseTooltipText.mText = gobj.GetName();

                        //select if clicked on
                        if (Input.GetTriggered(0, "MouseLClick") == 1.0f)
                        {
                            Application.Current.Dispatcher.Invoke(new Action(() =>
                            {
                                try
                                {
                                    if (Input.GetHeld(0, "AdditiveMod") == 1.0f)
                                    {
                                        GameObjectSelectionManager.RequestAdditiveSelect(gobj);
                                    }
                                    else
                                    {
                                        GameObjectSelectionManager.RequestSelect(gobj);
                                    }
                                }
                                catch (Exception e)
                                {
                                    Console.WriteLine("Consumed Error in Hierarchy: " + e.ToString());
                                }
                            }
                            ));
                        }
                    }
                    else
                    {
                        mouseTooltipText.mText = "";
                    }

                    //rotate camera and move camera
                    float dt = FrameController.getInstance().GetDeltaTime();
                    if (Input.GetHeld(0, "MoveFaster") == 1.0f)
                        dt *= 3;
                    Vector3 velocityThisFrame = new Vector3();
                    if (Input.GetHeld(0, "MouseRCLick") == 1.0f)
                    {
                        float dx = 0.25f * (Input.GetMouseDeltaX() * (3.14f / 180.0f));
                        float dy = 0.25f * (Input.GetMouseDeltaY() * (3.14f / 180.0f));
                        editorCamera.Pitch(dy);
                        editorCamera.RotateY(dx);

                        float val = Input.GetValue(0, "up");
                        velocityThisFrame.Z += val;
                        val = Input.GetValue(0, "right");
                        velocityThisFrame.X += val;
                        val = Input.GetValue(0, "down");
                        velocityThisFrame.Z -= val;
                        val = Input.GetValue(0, "left");
                        velocityThisFrame.X -= val;
                        cameraVelocity += velocityThisFrame;
                        //cameraVelocity = cameraVelocity.Add(velocityThisFrame);
                        //MathHelper.Clamp
                        cameraVelocity.X = MathHelper.Clamp(cameraVelocity.X, -6, 6);
                        cameraVelocity.Y = MathHelper.Clamp(cameraVelocity.Y, -6, 6);
                        cameraVelocity.Z = MathHelper.Clamp(cameraVelocity.Z, -6, 6);
                        //cameraVelocity.Clamp(-6.0f, 6.0f);
                    }
                    if (velocityThisFrame.X == 0)
                        cameraVelocity.X *= 0.9f;
                    if (velocityThisFrame.Z == 0)
                        cameraVelocity.Z *= 0.9f;
                    editorCamera.Walk(cameraVelocity.Z * dt);
                    editorCamera.Strafe(cameraVelocity.X * dt);

                    //look at
                    if (_lookTimer != 0)
                        _lookTimer += FrameController.DT();
                    if (_lookTimer > 1.3f)
                        _lookTimer = 0;
                    if (Input.GetTriggered(0, "LookAt") == 1.0f)
                    {
                        if (EngineManagerViewModel.instance.SelectedGameObjects.Count == 0)
                            return;
                        GameObject selobj = EngineManagerViewModel.instance.SelectedGameObjects[0];
                        if (_lookTimer == 0)
                        {
                            _lookTimer = 1; //force the setup
                            if (selobj != null)
                                editorCamera.gameObject.transform.LookAt(selobj.transform.position);
                        }
                        else
                        {
                            editorCamera.gameObject.transform.LookAt(selobj.transform.position);
                            Vector3 forw = editorCamera.gameObject.transform.GetForwardVector();
                            editorCamera.gameObject.transform.SetPosition(selobj.transform.position - (forw * 8));
                            _lookTimer = 0;
                        }
                    }

                    //arrow keys moving
                    if (Input.GetTriggered(0, "Add") > 0)
                        arrowSnapPrecision += 0.1f;
                    if (Input.GetTriggered(0, "Sub") > 0)
                        arrowSnapPrecision -= 0.1f;
                    if (Input.GetTriggered(0, "ArrowUp") > 0)
                        foreach (var selobj in EngineManagerViewModel.instance.SelectedGameObjects)
                            selobj.transform.SetPositionZ(selobj.transform.position.z + arrowSnapPrecision);
                    if (Input.GetTriggered(0, "ArrowDown") > 0)
                        foreach (var selobj in EngineManagerViewModel.instance.SelectedGameObjects)
                            selobj.transform.SetPositionZ(selobj.transform.position.z - arrowSnapPrecision);
                    if (Input.GetTriggered(0, "ArrowRight") > 0)
                        foreach (var selobj in EngineManagerViewModel.instance.SelectedGameObjects)
                            selobj.transform.SetPositionX(selobj.transform.position.x + arrowSnapPrecision);
                    if (Input.GetTriggered(0, "ArrowLeft") > 0)
                        foreach (var selobj in EngineManagerViewModel.instance.SelectedGameObjects)
                            selobj.transform.SetPositionX(selobj.transform.position.x - arrowSnapPrecision);
                }
            }
        }

        #region DESCRIPTORS
        ~EditorSubsystemManaged() { throw new InvalidOperationException("Cannot destroy singleton"); }

        NativeMochaDotNetCallback _tick;
        public void RegisterTick()
        {
            _tick = Tick;
            EditorSubsystem.getInstance().Register_Tick(_tick);

            //hack: for now, bind data direct after embed
            EngineCommandBindings.GridSize = EngineCommandBindings.GridSize;
        }

        /// <summary>
        /// Singleton access
        /// </summary>
        static EditorSubsystemManaged instance = null;
        public static EditorSubsystemManaged getInstance()
        {
            if (instance == null)
                instance = new EditorSubsystemManaged();
            return instance;
        }
        #endregion

    }
}
