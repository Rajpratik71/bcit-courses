﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace BankLibrary
{
    public partial class BankUIForm : Form
    {
        protected int TextBoxCount = 4;

        public enum TextBoxIndices
        {
            ACCOUNT,
            FIRST,
            LAST,
            BALANCE
        } // end enum

        public BankUIForm()
        {
            InitializeComponent();
        }

        public void ClearTextBoxes()
        {
            // iterate through every Control on form
            for (int i = 0; i < Controls.Count; i++)
            {
                Control myControl = Controls[i]; // get control

                // determine whether Control is TextBox
                if (myControl is TextBox)
                {
                    // clear Text property ( set to empty string )
                    myControl.Text = "";
                } // end if
            } // end for
        } // end method ClearTextBoxes

        public void SetTextBoxValues(string[] values)
        {
            // determine whether string array has correct length
            if (values.Length != TextBoxCount)
            {
                // throw exception if not correct length
                throw (new ArgumentException("There must be " +
                   (TextBoxCount + 1) + " strings in the array"));
            } // end if
            // set array values if array has correct length
            else
            {
                // set array values to text box values
                accountTextBox.Text = values[(int)TextBoxIndices.ACCOUNT];
                firstNameTextBox.Text = values[(int)TextBoxIndices.FIRST];
                lastNameTextBox.Text = values[(int)TextBoxIndices.LAST];
                balanceTextBox.Text = values[(int)TextBoxIndices.BALANCE];
            } // end else
        } // end method SetTextBoxValues

        public string[] GetTextBoxValues()
        {
            string[] values = new string[TextBoxCount];

            // copy text box fields to string array
            values[(int)TextBoxIndices.ACCOUNT] = accountTextBox.Text;
            values[(int)TextBoxIndices.FIRST] = firstNameTextBox.Text;
            values[(int)TextBoxIndices.LAST] = lastNameTextBox.Text;
            values[(int)TextBoxIndices.BALANCE] = balanceTextBox.Text;

            return values;
        } // end method GetTextBoxValues

    }
}
