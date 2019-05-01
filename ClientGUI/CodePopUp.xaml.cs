////////////////////////////////////////////////////////////////////////////////
//  CodePopUp.xaml.cs   -  A window to display the result or file content     //
//  ver 1.0                                                                   //
//  Language:         Visual C#  2017                                         //
//  Platform:         Lenovo Z580 Windows 10                                  //
//  Application :     FederationComm , FL17                                   //
// Author : Dushyant Sheoran, CSE687   							              //
//			dsheoran@syr.edu +1(704)-957-2519					              //
////////////////////////////////////////////////////////////////////////////////

using System.Windows;

namespace RepositoryService
{
    public partial class CodePopUp : Window
    {
        private static double leftOffset = 500.0;
        private static double topOffset = -20.0;

        public CodePopUp()
        {
            InitializeComponent();
            double Left = Application.Current.MainWindow.Left;
            double Top = Application.Current.MainWindow.Top;
            this.Left = Left + leftOffset;
            this.Top = Top + topOffset;
            this.Width = 600.0;
            this.Height = 800.0;
            leftOffset += 20.0;
            topOffset += 20.0;
            if (leftOffset > 700.0)
                leftOffset = 500.0;
            if (topOffset > 180.0)
                topOffset = -20.0;
        }

        public void showCode(string codeText)
        {
            // deprecated - will remove in later version
        }
        private void exitButton_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
    }
}
