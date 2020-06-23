using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;

namespace Unholy_Survivor___Reckoning
{
    public partial class Form1 : Form
    {
        const string GameExecutableName = "Game.exe";

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            // TODO: detect available resolution and remember last settings
            comboBoxResolution.Text = "1280x720";
        }

        private void buttonStartGame_Click(object sender, EventArgs e)
        {
            string[] resolution = comboBoxResolution.Text.Split('x');

            List<string> args = new List<string>();
            args.Add("--width " + resolution[0]);
            args.Add("--height " + resolution[1]);
            if (checkBoxFullscreen.Checked)
                args.Add("--fullscreen");

            ProcessStartInfo psi = new ProcessStartInfo(GameExecutableName);
            psi.Arguments = string.Join(" ", args.ToArray());
            psi.UseShellExecute = true;

            // TODO: OpenAL install

            try
            {
                Process.Start(psi);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Cannot launch game", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

            Application.Exit();
        }
    }
}
