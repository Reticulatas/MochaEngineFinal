using System;

namespace MochaEditor
{
    public class Material_Wrapper
    {
        //AssetHandle texture

        Material mat;

        public Vector2 tex_off {
            get ; set;}
        public Vector2 tex_scale;

        public Material_Wrapper(Material m)
        {
            mat = m;
        }
    }
}

