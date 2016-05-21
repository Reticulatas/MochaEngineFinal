using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MochaEngine
{
    class Program
    {
        static void Main(string[] args)
        {
            MochaCore.CoffeeConstructor.StartFromConfig();

            MochaCore.CoffeeConstructor.Loop();
        }
    }
}
