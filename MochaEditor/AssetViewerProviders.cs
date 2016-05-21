using System;
using System.Collections.Generic;
using System.IO;

namespace MochaEditor
{
    namespace AssetViewer
    {
        public class Item
        {
            public string Name { get; set; }
            public string Path { get; set; }
        }

        public class FileItem : Item { }

        public class DirectoryItem : Item
        {
            public List<Item> Items { get; set; }

            public DirectoryItem()
            {
                Items = new List<Item>();
            }
        }

        public class ItemProvider
        {
            public List<Item> GetItems(string path)
            {
                var items = new List<Item>();
                var dirInfo = new DirectoryInfo(path);
                foreach (var dir in dirInfo.GetDirectories())
                {
                    var item = new DirectoryItem
                    {
                        Name = dir.Name,
                        Path = dir.FullName,
                        Items = GetItems(dir.FullName)
                    };
                    items.Add(item);
                }

                foreach (var file in dirInfo.GetFiles())
                {
                    var item = new FileItem
                    {
                        Name = file.Name,
                        Path = file.FullName
                    };
                    items.Add(item);
                }

                return items;
            }
        }
    }
}



