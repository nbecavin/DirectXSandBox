using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;

namespace DataExport
{
    class ConvertMesh
    {
        [Category("Format")]
        [DefaultValue(true)]
        public Boolean SDKMesh { get; set; }

        [Category("Format")]
        [DefaultValue(true)]
        public Boolean WindowsD3D10 { get; set; }

        [Category("Format")]
        [DefaultValue(false)]
        public Boolean XBox360 { get; set; }

        public ConvertMesh()
        {
            // Iterate through each property
            foreach (PropertyDescriptor prop in TypeDescriptor.GetProperties(this))
            {
                // Set default value if DefaultValueAttribute is present
                DefaultValueAttribute attr = prop.Attributes[typeof(DefaultValueAttribute)] as DefaultValueAttribute; 
                if (attr != null)
                    prop.SetValue(this, attr.Value);
            }
        }

        public void Export(string fileName,string outfileName,string arguments = "")
        {
            System.Diagnostics.Process proc = new System.Diagnostics.Process();
            proc.StartInfo.FileName = "contentexporter.exe";

            string cmdline;
            cmdline = fileName + " ";
            cmdline += "-outputpath " + outfileName + " ";
            if(SDKMesh)
                cmdline += "-sdkmesh " + " ";

            if(WindowsD3D10)
                cmdline += "-windowsd3d10 " + " ";

            cmdline += "-compressednormaltype " + "float16_4" + " ";

            proc.StartInfo.Arguments = cmdline;

            //try
            {
                proc.Start();
            }
            //catch (System.ComponentModel.Win32Exception)
            {
            }
            //try
            {
                proc.WaitForExit();
            }
            //catch (System.InvalidOperationException)
            {
            }
        }
    }
}
