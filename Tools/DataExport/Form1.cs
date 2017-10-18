using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace DataExport
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();

            this.convMesh = new ConvertMesh();
            this.propertyGrid1.SelectedObject = this.convMesh; 
        }

        private ConvertMesh convMesh;

        private void button1_Click(object sender, EventArgs e)
        {
            this.openFBXDlg.ShowDialog();
            this.inputFilePath.Text = this.openFBXDlg.FileName;
        }

        private void exportButton_Click(object sender, EventArgs e)
        {
            this.convMesh.Export(this.inputFilePath.Text,this.outFilePath.Text);
        }

        private void button1_Click_1(object sender, EventArgs e)
        {
            this.saveFileDlg.ShowDialog();
            this.outFilePath.Text = this.saveFileDlg.FileName;
        }
    }
}
