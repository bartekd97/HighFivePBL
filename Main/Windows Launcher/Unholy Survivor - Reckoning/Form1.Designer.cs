namespace Unholy_Survivor___Reckoning
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.panel1 = new System.Windows.Forms.Panel();
            this.comboBoxResolution = new System.Windows.Forms.ComboBox();
            this.label2 = new System.Windows.Forms.Label();
            this.checkBoxFullscreen = new System.Windows.Forms.CheckBox();
            this.buttonStartGame = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // panel1
            // 
            this.panel1.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("panel1.BackgroundImage")));
            this.panel1.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.panel1.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel1.Location = new System.Drawing.Point(0, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(413, 184);
            this.panel1.TabIndex = 0;
            // 
            // comboBoxResolution
            // 
            this.comboBoxResolution.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBoxResolution.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.comboBoxResolution.FormattingEnabled = true;
            this.comboBoxResolution.Items.AddRange(new object[] {
            "800x600",
            "1024x768",
            "1280x720",
            "1366x768",
            "1440x900",
            "1600x900",
            "1920x1080",
            "2560x1440"});
            this.comboBoxResolution.Location = new System.Drawing.Point(214, 202);
            this.comboBoxResolution.Name = "comboBoxResolution";
            this.comboBoxResolution.Size = new System.Drawing.Size(134, 26);
            this.comboBoxResolution.TabIndex = 1;
            // 
            // label2
            // 
            this.label2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.label2.Location = new System.Drawing.Point(9, 201);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(199, 26);
            this.label2.TabIndex = 2;
            this.label2.Text = "Screen resolution:";
            this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // checkBoxFullscreen
            // 
            this.checkBoxFullscreen.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.checkBoxFullscreen.Location = new System.Drawing.Point(214, 234);
            this.checkBoxFullscreen.Name = "checkBoxFullscreen";
            this.checkBoxFullscreen.Size = new System.Drawing.Size(121, 30);
            this.checkBoxFullscreen.TabIndex = 3;
            this.checkBoxFullscreen.Text = "Fullscreen";
            this.checkBoxFullscreen.UseVisualStyleBackColor = true;
            // 
            // buttonStartGame
            // 
            this.buttonStartGame.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.buttonStartGame.Location = new System.Drawing.Point(12, 296);
            this.buttonStartGame.Name = "buttonStartGame";
            this.buttonStartGame.Size = new System.Drawing.Size(389, 32);
            this.buttonStartGame.TabIndex = 4;
            this.buttonStartGame.Text = "Start !";
            this.buttonStartGame.UseVisualStyleBackColor = true;
            this.buttonStartGame.Click += new System.EventHandler(this.buttonStartGame_Click);
            // 
            // Form1
            // 
            this.AcceptButton = this.buttonStartGame;
            this.AutoScaleDimensions = new System.Drawing.SizeF(120F, 120F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.AutoValidate = System.Windows.Forms.AutoValidate.EnablePreventFocusChange;
            this.ClientSize = new System.Drawing.Size(413, 340);
            this.Controls.Add(this.buttonStartGame);
            this.Controls.Add(this.checkBoxFullscreen);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.comboBoxResolution);
            this.Controls.Add(this.panel1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "Form1";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Unholy Survivor: Reckoning";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.ComboBox comboBoxResolution;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.CheckBox checkBoxFullscreen;
        private System.Windows.Forms.Button buttonStartGame;
    }
}

