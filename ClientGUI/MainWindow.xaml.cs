////////////////////////////////////////////////////////////////////////////////
//  MainWindow.xaml.cs   -  We can state number of child process to run       //
//                          can be navigate to Build Request Window to create //
//                          build request                                     //
//  ver 1.0                                                                   //
//  Language:         Visual C#  2017                                         //
//  Platform:         Lenovo Z580 Windows 10                                  //
//  Application :     FederationComm , FL17                                   //
// Author : Dushyant Sheoran, CSE687   							              //
//			dsheoran@syr.edu +1(704)-957-2519					              //
////////////////////////////////////////////////////////////////////////////////
/* 
 * Modular Operations
 =====================
 *MainWindow of Client GUI that provides capability to select number of process and to create Build Request by 
 * navigating to the Build Request window.
 * 
 * public:
 * ===========
 * window_loaded       :  The Window that gets loaded when Client starts Execution thiis helps to
 *                         simulate client processing such as button clicks
 * BoolStringClass     :  A helper class for datatemplate
 * processMessages     :  Client's reciever thread that recieves messages from recieve queue
 *                         usig the translater sd the medium
 * clearDict           : A helper function to clear the dictionary list
 * populateFromDicts   : Helper function to get cateories and dependencies
 * addStorageFile      :
 * addClientProc       : To add the process of client to the the client processing dictionary/dispatcher
 * writeStatus         : Write the status to the status bar.
 * DispatecherLoadCheckin : to register the check-in functionality to the the client process dispatcher
 *sendFileRequest         : The client sends the check-out request to the Repository server
 *                          on processing the check-out reply from the client 
 * DispatecherLoadCheckout : The check-out processing registering the delegate to dispatcher
 * DispatcherLoadGetDirs   : The loadfiledirs registering the delegate to the dispatcher.
 * DispatecherLoadConnection : The connection tab setup with the adddress of the machine and the port
 *                              numbers
 * connectionsetup      : Establishing the connection setup the machine address and port number
 * checkinClick         : That sends the checkin message to the repository server.
 * checkoutClick        : That sends the check-out message to the repository server.
 * loadDispatcher       : Registering the client process to the client dispatcher
 * testcheckin          : To demonstrate check-in functionality
 * RequirementIdea       : To sho how the given requirements are satisfied
 * testBrosweDoubleClick : To implement the same without button click
 * DirList_MouseDoubleClick : Has a funcitonality to navigate over the directories in Repository
 *                           
 *  Build Process:
 *  ================
 *  Required files:
 *  -------------------
 *  MainWindow.xaml.cs; App.xaml; CodePopUp.xaml; MainWindow.xaml;CodePopUp.xaml.
 *  
 *  Compiler Command
 *  =======================
 *  csc /target:exe /define:ClientGUI
 *    
 *  Maintenanace History:
 *  ======================
 *  ver 1.0
 *  */
using System;
using System.Collections.Generic;
using System.Windows;
using System.IO;
using System.Threading;
using System.Windows.Controls;
using System.Text;
using System.Runtime.InteropServices;
using System.Diagnostics;
using MsgPassingCommunication;
using System.Linq;
using System.Collections.ObjectModel;
using Microsoft.Win32;
using System.Windows.Documents;

#pragma warning disable CS2002

namespace RepositoryService
{
    public partial class MainWindow : Window
    {
        String[] args { get; set; }
        List<Window> popups { get; set; } = null;
        String path { get; set; }
        String _filestatus { get; set; }
        String _catogries { get; set; }
        String dependecies { get; set; }
        String _fileName { get; set; }
        String _filePath { get; set; }
        String nameSpaceInfo { get; set; }
        public ObservableCollection<BoolStringClass> TheList { get; set; }
        public ObservableCollection<BoolStringClass> TheCat { get; set; }
        public ObservableCollection<BoolStringClass> TheStat { get; set; }
        public object AppSettings { get; private set; }
        public object ConfigurationManager { get; private set; }

        enum DisplayMode { nameSpace, files }
        DisplayMode chkInDisplayMode = DisplayMode.nameSpace;

        enum ViewMode { code, metadata }


        public MainWindow()
        {
            InitializeComponent();
            popups = new List<Window>();
            args = Environment.GetCommandLineArgs();
            Console.Title = "ClientGUI Window";
            args = Environment.GetCommandLineArgs();
        }
        private CsEndPoint endPoint_;
        private Thread rcvThrd = null;
        private Translater translater;
        private Dictionary<string, Action<CsMessage>> dispatcher_ = new Dictionary<string, Action<CsMessage>>();
        private CsEndPoint serverEndPoint;
        private Dictionary<string, string> attributesInfo = new Dictionary<string, string>();
        private Stack<string> pathStack_ = new Stack<string>();
        private Dictionary<int, String> catDict = new Dictionary<int, string>();
        private Dictionary<int, String> depDict = new Dictionary<int, string>();

        //----< process incoming messages on child thread >----------------
        public class BoolStringClass
        {
            public string TheText { get; set; }
            public int TheValue { get; set; }
        }
        //----------< Process messages by the clients using dictionary >--------------
        private void processMessages()
        {
            int flag = 0;
            ThreadStart thrdProc = () =>
            {
                while (true)
                {
                    CsMessage msg = translater.getMessage();
                    var enumer = msg.attributes.GetEnumerator();
                    while (enumer.MoveNext())
                    {

                        string key = enumer.Current.Key;
                        if (key == "command" && enumer.Current.Value == "")
                        {
                            flag = 1;
                            break;
                        }
                    }
                    if (flag == 1)
                        break;
                    string msgId = msg.value("command");
                    if (dispatcher_.ContainsKey(msgId))
                        dispatcher_[msgId].Invoke(msg);
                }
            };
            rcvThrd = new Thread(thrdProc);
            rcvThrd.IsBackground = true;
            rcvThrd.Start();
        }
        //-------------------< a fnction to handle categories and dependencies during check-in >---------
        private void clearDict()
        {
            catDict = new Dictionary<int, string>();
            depDict = new Dictionary<int, string>();
            return;
        }
        //------------------------< to get categories and dependencies information >-------
        private void populateFromDicts()
        {
            _filestatus = "";
            _catogries = "";
            dependecies = "";
            if (catDict.Count != 0)
            {
                foreach (KeyValuePair<int, String> cat in catDict)
                {
                    if (cat.Value != "")
                        _catogries = _catogries + cat.Value + "+";
                }
            }
            foreach (string dep in ChkInDeps.Items)
            {
                dependecies = dependecies + dep + "+";
            }
        }
        //------------------------------< Add files in storage >---------------------------------------
        private void addStorageFile(string fileName)
        {
            
        }

        //----< add client processing for message with key >---------------
        private void addClientProc(string key, Action<CsMessage> clientProc)
        {
            dispatcher_[key] = clientProc;
        }

        private void writeStatus(string status)
        {
            statusBarText.Text = status;
        }
        //----< function dispatched by child thread to main thread >-------

        private void clearDirs()
        {
            DirList.Items.Clear();
        }
        //----< function dispatched by child thread to main thread >-------

        private void addDir(string dir)
        {
            DirList.Items.Add(dir);
        }
        //----< function dispatched by child thread to main thread >-------

        private void insertParent()
        {
            DirList.Items.Insert(0, "..");
        }
        //----< function dispatched by child thread to main thread >-------

        private void clearFiles()
        {
            FileList.Items.Clear();
        }
        //----< function dispatched by child thread to main thread >-------

        private void addFileBro(string file)
        {
            FileList.Items.Add(file);
        }
        //------------------------------< Proceess check-in >------------------------
        private void DispatecherLoadCheckin()
        {
            Action<CsMessage> processCheckin = (CsMessage rcvMsg) =>
            {
                Action<string> _writeStatuscheckin = (string status) =>
                {
                    writeStatus(status);
                };
                Action<string> _serverMessageWrite = (string mess) =>
                  {
                      serverMessageWrite(mess);
                  };
                String msgtowrit = "Check-in: "+rcvMsg.value("fileName")+":"+rcvMsg.value("statuschckin");
                string message = "Check-in File Status from" + rcvMsg.value("from") + " to:" + rcvMsg.value("to") + " status: " + rcvMsg.value("statuschckin");
                Dispatcher.Invoke(_writeStatuscheckin, new string[] { msgtowrit });
                Dispatcher.Invoke(_serverMessageWrite, new string[] { message });
                Console.WriteLine("\n       Requirement #2b Check-in Reply\n==================================================");
                Console.WriteLine("\nRecieved a check-in status");
                rcvMsg.show();
                
            };
            addClientProc("check-in", processCheckin);
        }
        //-----------------< Helper function to display metadata info of the file >----------
        private void metadata(string info)
        {
            FileInfo.Items.Add(info);
        }
        //---------------------< Delegater to clear files >---------------
        private void _clearFileInfo()
        {
            FileInfo.Items.Clear();
        }
        //-------------------< Delegater to Dependents info to metadata >---------------
        private void depen(String depends)
        {
            List<string> deps = new List<string>(depends.Split('+'));
            foreach (string s in deps)
                Dependents.Items.Add(s);
        }
        //----------------< Process Query results >------------------------
        private void QueryResults(String res)
        {
            queryResults.Items.Clear();
            List<string> deps = new List<string>(res.Split('+'));
            foreach (string s in deps)
                queryResults.Items.Add(s);
        }
        //-------------< Deletager to clear the list box >--------------------
        private void clearDepe()
        {
            Dependents.Items.Clear();
        }
        //--------------------< Ask for requested files >-------------------------
        private void sendFileRequest(CsMessage rcvdMessage)
        {
            path = Path.GetFullPath("../../../Stagging");
            CsMessage frequest = new CsMessage();
            frequest.add("to", CsEndPoint.toString(serverEndPoint));
            frequest.add("from", CsEndPoint.toString(endPoint_));
            frequest.add("command", "filesCopy");
            frequest.add("messageText", "Send Files to Checkout Path");
            frequest.add("path", path);
            frequest.add("fileNeeded", rcvdMessage.value("fileName"));
            translater.postMessage(frequest);
            List<string> deps = new List<string>(rcvdMessage.value("Dependecy").Split('+'));
            foreach (string filename in deps)
            {
                CsMessage frequest1 = new CsMessage();
                frequest1 = frequest;
                frequest1.remove("command");
                frequest1.add("command", "filesCopy");
                frequest1.remove("fileNeeded");
                frequest1.add("fileNeeded", filename);
                translater.postMessage(frequest1);
            }
            return;
        }
        //-------------------< Writes messages to server Message >----------------------
        private void serverMessageWrite(string message)
        {
            Server_Message.Items.Add(message);
        }
        //-----------------< Helper function >-----------
        private void updateMessageHelper(CsMessage rcvMsg)
        {
            Action<string> updateServerMessage = (string message) =>
            {
                serverMessageWrite(message);
            };
            string message1 = "Check-out File status from:" + rcvMsg.value("from") + " to :" + rcvMsg.value("to");
            string message2 = "Check -out status: " + rcvMsg.value("check-out_status");

            Dispatcher.Invoke(updateServerMessage, new string[] { message1 });
            Dispatcher.Invoke(updateServerMessage, new string[] { message2 });

        }
        //----------------< Process the message >------------------
        private void processMessages(CsMessage rcvMsg)
        {
            List<string> metainf = new List<string> { "name", "Description", "FilePath", "category" };
            foreach (KeyValuePair<string, string> see in rcvMsg.process1())
            {
                Action<string> _metadata = (string info) =>
                {
                    metadata(info);
                };
                Action<string> _depen = (string info) =>
                {
                    depen(info);
                };
                if ((see.Key).Equals("Dependecy"))
                {
                    Dispatcher.Invoke(_depen, new string[] { see.Value });
                }
                else if (metainf.Any((see.Key).Contains))
                    Dispatcher.Invoke(_metadata, new string[] { see.Key + "  :  " + see.Value });
                
            }
            updateMessageHelper(rcvMsg);
          
        }
        //--------------------< Process Checkout >-----------------------
        private void DispatecherLoadCheckout()
        {
            List<string> metainf = new List<string> { "name", "Description", "FilePath", "category" };
            Action<CsMessage> processCheckout = (CsMessage rcvMsg) =>
            {
                Console.WriteLine("\n       Requirement #2b Check-out Reply\n==================================================");
                Console.WriteLine("\nRecieved a check-out status");
                Console.WriteLine("\n The files can be seen in ClientStorage");
                rcvMsg.show();
                Action<string> _writeStatusCheckout = (string status) =>
                {
                    writeStatus(status);
                };
                String msgtowrite = rcvMsg.value("check-out_status");
                Dispatcher.Invoke(_writeStatusCheckout, new string[] { msgtowrite });
                Action clearFileInfo = () =>
                {
                    _clearFileInfo();
                };
                Action _clearDepe = () =>
                {
                    clearDepe();
                };

                Dispatcher.Invoke(_clearDepe, new Object[] { });
                Dispatcher.Invoke(clearFileInfo, new Object[] { });
                processMessages(rcvMsg);

            };
            addClientProc("check-out", processCheckout);
        }
        //---------------------------< To loadFile info>-------------------
        private void DispatecherLoadDownloadFile()
        {
           
            Action<CsMessage> processFileDownload = (CsMessage rcvMsg) =>
            {
                Console.WriteLine("\n       Requirement #6 and #3 FileDownload Reply\n==================================================");
                Console.WriteLine(" File Downloaded on check-out request from CheckoutStorage to ClientStorage ");
                rcvMsg.show();
                Action<string> fileDoenloadMessage = (string message) =>
                {
                    serverMessageWrite(message);
                };
                string messsage = "File Download from:" + rcvMsg.value("from") + "to:" + rcvMsg.value("to");
                Dispatcher.Invoke(fileDoenloadMessage, new Object[] { messsage });
            };
            addClientProc("finalMessage", processFileDownload);
        }




        //--------------------< Custom Queries >-----------------------
        private void DispatcherLoadCustomQueries()
        {
          
            Action<CsMessage> processcustomQuery = (CsMessage rcvMsg) =>
            {
                Console.WriteLine("\n       Requirement #2b Custom Query Reply\n==================================================");
                Console.WriteLine("\nSupport Custom queries on Category Check-in status file's present Version will also get Dependencies ");
                rcvMsg.show();
                Action<string> _writeStatusQuery = (string status) =>
                {
                    writeStatus(status);
                };
                Action<string> resDisplay = (string res) =>
                {
                    QueryResults(res);
                };
                Action<string> _writeServerMessage = (string mess) =>
                  {
                      serverMessageWrite(mess);
                  };
                String result = rcvMsg.value("queryresult");
                String msgtowrite = rcvMsg.value("queryStatus");
                string message = "Custom Querry Results on "+rcvMsg.value("type")+" from:" + rcvMsg.value("from") + " to:" + rcvMsg.value("to") + " " + rcvMsg.value("status");
                Dispatcher.Invoke(_writeStatusQuery, new string[] { msgtowrite });
                Dispatcher.Invoke(resDisplay, new string[] { result });
                Dispatcher.Invoke(_writeServerMessage, new string[] { message });
            };
            addClientProc("customQuery", processcustomQuery);
        }
        //------------------< helper function to clear dirs list box>-------------
        private void cleandirs()
        {
            Action clrDirs = () =>
            {
                clearDirs();
            };
            Dispatcher.Invoke(clrDirs, new Object[] { });
            return;
        }
        //----------------< helper function to write messages to message flow list box>------------------
        private void writemessage(string message)
        {
            Action<string> _serverMessageWrite = (string mess) =>
            {
                serverMessageWrite(mess);
            };
            Dispatcher.Invoke(_serverMessageWrite, new Object[] { message });
            return;
        }
        //---------< Load Directories >-------------------------
        private void DispatcherLoadGetDirs()
        {
            Action<CsMessage> getDirs = (CsMessage rcvMsg) =>
            {
                Console.WriteLine("\n           Requirement #2c       ");
                Console.WriteLine("-----------------------------------------------------------------------------------------------=");
                Console.WriteLine("Recieved path of current browse directorry info Reply from Repository with the List of Directories");
                rcvMsg.show();
                cleandirs();
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("dir"))
                    {
                        Action<string> doDir = (string dir) =>
                        {
                            addDir(dir);
                        };
                        Dispatcher.Invoke(doDir, new Object[] { enumer.Current.Value });
                    }
                }
                Action insertUp = () =>
                {
                    insertParent();
                };
                Action<string> _writeStatusDirs = (string status) =>
                {
                    writeStatus(status);
                };
                string _status = "Get Dirs status:" + rcvMsg.value("status");
                string message = "Get Dirs received from:" + rcvMsg.value("from") + " to:" + rcvMsg.value("to") + " status: " + rcvMsg.value("status");
                Dispatcher.Invoke(insertUp, new Object[] { });
                writemessage(message);
                Dispatcher.Invoke(_writeStatusDirs, _status);
            };
            addClientProc("getDirs", getDirs);
        }
        //----< load getFiles processing into dispatcher dictionary >------

        private void DispatcherLoadGetFiles()
        {
            Action<CsMessage> getFiles = (CsMessage rcvMsg) =>
            {
                Console.WriteLine("\n           Requirement #2c       ");
                Console.WriteLine("----------------------------------------------------------------=");
                Console.WriteLine("Recieved getFile Reply from Repository with the List of Files");
                rcvMsg.show();
                Action clrFiles = () =>
                {
                    clearFiles();
                };
                Dispatcher.Invoke(clrFiles, new Object[] { });
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("file"))
                    {
                        Action<string> doFile = (string file) =>
                        {
                            addFileBro(file);
                        };
                        Dispatcher.Invoke(doFile, new Object[] { enumer.Current.Value });
                    }
                  
                   
                }
                Action<string> _serverMessageWrite = (string mess) =>
                {
                    serverMessageWrite(mess);
                };

                string message = "Get Files received from:" + rcvMsg.value("from") + " to:" + rcvMsg.value("to") + " status: " + rcvMsg.value("status");

                Dispatcher.Invoke(_serverMessageWrite, new Object[] { message });

            };
            addClientProc("getFiles", getFiles);
        }

//-------------< Clear Metadata Info >-------
        private void clearDepListBox()
        {
            Dependen.Items.Clear();
        }
        private void addDeps(string depends)
        {
            Dependen.Items.Add("Children");
            Dependen.Items.Add("--------------");
            List<string> deps = new List<string>(depends.Split('+'));
            foreach (string s in deps)
                Dependen.Items.Add(s);
        }
        private void addMetaInf(string metaInf)
        {
            Dependen.Items.Add(metaInf);
        }
        private string messageHelper(CsMessage rcvMsg)
        {
            List<string> metainf = new List<string> { "Author", "Description", "status", "category" };
            string collectMetadata = "";
            foreach (KeyValuePair<string, string> see in rcvMsg.process1())
            {
                Action<string> _metadata = (string info) =>
                {
                    addMetaInf(info);
                };
                Action<string> _addDeps = (string deps) =>
                {
                    addDeps(deps);
                };
                if ((see.Key).Equals("Dependecy"))
                {
                    Dispatcher.Invoke(_addDeps, new string[] { see.Value });
                    collectMetadata = collectMetadata + "\n" + see.Value;
                }
                else if (metainf.Any((see.Key).Contains))
                {
                    Dispatcher.Invoke(_metadata, new string[] { see.Key + "  :  " + see.Value });
                    collectMetadata = collectMetadata + "\n" + see.Key + ":" + see.Value;
                }
            }

            return collectMetadata;
        }
        //---------------<Request for file Content >-----------
        private void DispatcherLoadContentRequest()
        {
            Action<CsMessage> viewFile = (CsMessage rcvMsg) =>
            {
                Console.WriteLine("\n           Requirement ##       ");
                Console.WriteLine("----------------------------------------------------------------=");
                Console.WriteLine("Recieved a File Content View Message");
                rcvMsg.show();
                string collectMetadata="";
                Action clearDeps = () =>
                {
                    clearDepListBox();
                };
                Dispatcher.Invoke(clearDeps, new Object[] { });
                collectMetadata = messageHelper(rcvMsg);
                string fileName= Path.Combine(rcvMsg.value("path"), rcvMsg.value("fileName"));

                fileName = fileName + "@" + collectMetadata;
                Action<string> showFileHelper = (string name) =>
                {
                    showFileContent(name);
                };
                Dispatcher.Invoke(showFileHelper, new string[] { fileName });
            };
            addClientProc("fileContent", viewFile);
        }

        //-----------------------< Process Connection >----------------
        private void DispatecherLoadConnection()
        {
            Action<string> _updateString = (string statusMess) =>
            {
                writeStatus(statusMess);
            };
            Dispatcher.Invoke(_updateString, new String[] { "This is status bar to show messageText" });
            Action<CsMessage> processConnection = (CsMessage rcvMsg) =>
            {
                Console.WriteLine("\n           Requirement #5         ");
                Console.WriteLine("========================================");
                Console.WriteLine("Recieved a connection reply but asynchronous");
                rcvMsg.show();
                Action<string> _writeStatus = (string statuswrite) =>
                {
                    writeStatus(statuswrite);
                };
                Dispatcher.Invoke(_writeStatus, new String[] { "Connection set-up : Success" });
            };
            addClientProc("Connection-set", processConnection);
        }
        //---------------< Helper function >------------
        private void DispatcherConnectCheck()
        {
            Action<CsMessage> pingFunc = (CsMessage rcvMsg) =>
            {

            };
            addClientProc("connectCheck", pingFunc);
        }
        //------------------< handle file request >----------
        private void DispatcherSendFileRequest()
        {
            Action<CsMessage> sendFileProcess = (CsMessage rcvMsg) =>
            {
                Console.WriteLine(" \n   Requirement #2 and #6 Recieved a SendFile/\"FileUpLoad\" Request message");
                Console.WriteLine("==============================================================");
                Console.WriteLine("\nRecieved a FileRequest during check-in from Server : \"FileUpLoad\"\n");    
                CsMessage reply = new CsMessage();
                reply.add("to", CsEndPoint.toString(serverEndPoint));
                reply.add("from", CsEndPoint.toString(endPoint_));
                reply.remove("command");
                reply.add("command", "file-status");
                reply.add("file", Path.GetFileName(rcvMsg.value("fileName")));
                reply.add("sendFilePath", "../../../Stagging");
                reply.add("saveFilePath", "../CheckinStorage");
                reply.add("messageText", "File transfer success");
                reply.show();
                Action<string> _serverMseaageWrite = (string statuswrite) =>
                {
                    serverMessageWrite(statuswrite);
                };
                string message = "File Upload from: " + reply.value("from") + " to: " + reply.value("to") + " file Name " + reply.value("file");
                Action<string> _writeStatus = (string statuswrite) =>
                {
                    writeStatus(statuswrite);
                };
                Dispatcher.Invoke(_writeStatus, new String[] { "Uploading File" });
                Dispatcher.Invoke(_serverMseaageWrite, new String[] {message });
                translater.postMessage(reply);
                
            };
            addClientProc("sendFile", sendFileProcess);
        }
        //----------------------< File status handler >-----------------------
        private void DispatcherFileStatus()
        {

            Action<CsMessage> fileStatusMessage = (CsMessage rcvMsg) =>
            {
                Console.WriteLine("\n           Requirement #2b        ");
                Console.WriteLine("-------------------------------------------------------------=");
                Console.WriteLine("Recieved a File :: file with Status From Repository Server");
                rcvMsg.show();
            };

            addClientProc("file-status", fileStatusMessage);
        }
        //---------------------<On recieving quit mesage client swhould close by iteslf - comming up in Project #4>-----
        private void DispatcherQuit()
        {

            Action<CsMessage> fileStatusMessage = (CsMessage rcvMsg) =>
            {
                Console.WriteLine("\n           Requirement ##      ");
                Console.WriteLine("-------------------------------------------------------------=");
                Console.WriteLine("Recieved a quit");
                rcvMsg.show();
            };

            addClientProc("serverQuit", fileStatusMessage);
        }

        //------------------------------------< Load Dispatcher with Key and functions >---------------------
        private void loadDispatcher()
        {
            DispatecherLoadCheckin();
            DispatecherLoadCheckout();
            DispatecherLoadConnection();
            DispatcherConnectCheck();
            DispatcherFileStatus();
            DispatcherLoadGetDirs();
            DispatcherLoadGetFiles();
            DispatcherSendFileRequest();
            DispatcherLoadCustomQueries();
            DispatcherLoadContentRequest();
            DispatecherLoadDownloadFile();
            DispatcherQuit();
        }
        //-----------------------< Requirement Helper >------------------
        private void requirementHelper4()
        {
            Console.WriteLine("\n-            Demonstrating Requirement #4      ");
            Console.WriteLine("GUI is another Process using Message Passing Comm communicating with Repo Server\n");

        }
        //--------------------< Requirement Helper >----------------------------
        private void requirementHelper7()
        {
            Console.WriteLine("\n            Demonstrating Requirement #7     ");
            Console.WriteLine("Provided automates tests by Simulating GUI Clicks and functionalities\n");

        }
        //---------------< Usage Description Helper >-----------------
        private void UsageDescription()
        {
            Console.WriteLine("   USage      ");
            Console.WriteLine("===============================");
            Console.WriteLine("Check-in File Tab helps to checkin File: Selection of by double-clicking on show Projects");
            Console.WriteLine("Made Directroies based on Project nameSpace");
            Console.WriteLine("Demonstrated Check-out with and without Dependencies");
            Console.WriteLine("In brose Tab Can View the File Conetent by double clicking on the File and it shows metadata info also");
            Console.WriteLine("To understand the message flow added the Message to and from client- server to Server Message Box of Connect Server Tab");
            Console.WriteLine("Metada Info also displayed in Check-out Tab");
            Console.WriteLine("Custom Serach shows the Search Based on the List Displayed");
            Console.WriteLine("CheckinStorage is the Remote Files Storage Space which the repostory treats as the Stagging Path");
            Console.WriteLine("Storage is the Final Destination for the Client where it will store with Version numbers");
            Console.WriteLine("The Directory classification specified based on namespace");
            Console.WriteLine("Check-out Storage is the place where Client dowloaded files are stored");
        }
        //-----------------------------< Started when GUI is called >---------------------------------------------------
        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            TheCat = new ObservableCollection<BoolStringClass>();
            Console.WriteLine("\n-            Demonstrating Requirement #1 WPF USAGE       ");
            Console.WriteLine("=============================================================");

            UsageDescription();
            initializeChkIn();
            requirementHelper4();
            requirementHelper7();
            
            if (args.Length == 5)
            {
                startRepoClinetTest();
                connectionHelper();
                loadStoraeFiles(sender, e);
                testCheckoutWithDep();
            }
            if (args.Length == 4)
            {
                connectionHelper();
                loadStoraeFiles(sender, e);
                testCheckoutWithDep();
                testcheckin(sender, e);
                testCheckIn1(sender, e);
                testcheckinClose(sender, e);
                testCheckinClose2(sender, e);
                testCheckinClose3(sender, e);
                testcheckinNewVersion(sender, e);
                testCheckinFailCase1(sender, e);
                testcheckinSample(sender, e);
                testcheckinSample2(sender, e);
                testCheckoutWithOutDep();
                BrowseSetup(sender, e);
                testCustomQueries();
                testCustomQueries2();
                testCustomQueries3();
                testCustomQueries4();
                FileContentDisplay();
              
            }


        }
        //-----------< To show that multiple clients are supported >---------
        private void startRepoClinetTest()
        {
            Console.Title = "Client GUI #2";
            Server_Message.Items.Clear();
            Server_Message.Items.Add("Client #2 Message Flow");
            Server_Message.Items.Add("=================");
            Console.WriteLine("This shows multiple client support");
            Console.WriteLine("all functionalitiies are not performed to avoid abiguity at server side during grading");

        }
        /*----< make msg list display string >-------------------------*/

        string makeMsgDisplayStr(CsMessage msg)
        {
            string display = msg.value("command") + " -- " + msg.value("to") + " " + DateTime.Now.ToString();
            return display;
        }

        /*----< display connection setup  messages >-----------------------------*/

        void displayOutGoingMsg(CsMessage msg)
        {
            Server_Message.Items.Insert(1, makeMsgDisplayStr(msg));
        }

        //---------------------< Sends the selected Build Request to the MotherBuild Server via Repository >-------
        private void connectionsetup(Object sender, RoutedEventArgs e)
        {
            Console.WriteLine("      Requirement #3a  Connection-setup tab Can be seen in GUI\n======================================================================\n");
            endPoint_ = new CsEndPoint();
            endPoint_.machineAddress = AddressGUI.Text;
            endPoint_.port = Int32.Parse(PortNumberGUI.Text);
            translater = new Translater();
            translater.listen(endPoint_);
            processMessages();
            loadDispatcher();
            serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = Address.Text;
            serverEndPoint.port = Int32.Parse(PortNumber.Text);
            Console.WriteLine("Server endpoint is{0}", serverEndPoint.machineAddress + ":" + serverEndPoint.port);
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "connect");
            msg.add("connectCheck", "Connection Message");
            displayOutGoingMsg(msg);
            translater.postMessage(msg);
        }
        //--------------< Connection Helper >---------------------------
        private void connectionHelper()
        {
            Console.WriteLine("      Requirement #3a  Connection-setup tab Can be seen in GUI\n======================================================================\n");
            endPoint_ = new CsEndPoint();
            endPoint_.machineAddress = AddressGUI.Text;
            endPoint_.port = int.Parse(args[2]);
            translater = new Translater();
            translater.listen(endPoint_);
            processMessages();
            loadDispatcher();
            serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = Address.Text;
            serverEndPoint.port = Int32.Parse(PortNumber.Text);
            Console.WriteLine("Server endpoint is{0}", serverEndPoint.machineAddress + ":" + serverEndPoint.port);
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "connect");
            msg.add("connectCheck", "Connection Message");
            translater.postMessage(msg);
        }
        /*----< loads Repository categories in ChkInDeps list >--------*/

        void initializeChkIn(bool changeEnabled = true)
         {
            populateCategories();
            String []categories = getProjectDirsToPopulate();
            DepChk.IsChecked = false;
             foreach (string cat in categories)
             {
                 RepoFiles.Items.Add(Path.GetFileName(cat));
             }
            chkInDisplayMode = DisplayMode.nameSpace;
            if (changeEnabled)
             {
                 BrowseButton.IsEnabled = false;
                 ChkInButton.IsEnabled = false;
             }
             ChkInMode.IsChecked = false;
             Instructions.Content = "First: dbl clk on target category below";
             ChkInCatName.Visibility = Visibility.Collapsed;
             statusBarText.Text = "Action: select checkin target directory by double clicking on category";
         }

        /*----< show categories in checkin view >----------------------*/

        private void ChkInCatsButton_Click(object sender, RoutedEventArgs e)
        {
            RepoFiles.Items.Clear();
            initializeChkIn(false);
        }
        //-------< Check-in Helper >--------------
        private void afterCheckin(CsMessage msg)
        {
            if (translater != null)
            {
                Server_Message.Items.Add("Check-in request from: " + msg.value("from") + " to: " + msg.value("to") + " file: " + msg.value("fileName"));
                Console.WriteLine("      Check-in Request     \n-----------------------------------------");
                msg.show();
            }
            clearDict();
            RepoFiles.Items.Clear();
            ChkInCatName.Text = "";
            ChkInDeps.Items.Clear();
            ChkInFile.Text = "";
            PkgDescript.Text = "";
            initializeChkIn();
            ChkInButton.IsEnabled = false;
            ChkInFile.Text = "";
            Instructions.Content = "First: select target category, below";
            statusBarText.Text = "Action: select checkin category by double clicking on category";
        }
        //------------------< Check-in Button Click >---------------------------
        private void ChkInButton_Click(object sender, RoutedEventArgs e)
        {
            Console.WriteLine("\n   Requirement #3b check-in click button simulation from Check-in Tab \n===============================================================");
            CsMessage msg = new CsMessage();
            populateFromDicts();
            String _authorName = (String)authorName.Text;
              if (_authorName == "")
               {
                   Console.WriteLine("Please Provide Author Name");
                   statusBarText.Text = "Author Name Can not be Empty";
                   return;
               }
             String desc = (String)PkgDescript.Text;
              if (desc == "")
              {
                  Console.WriteLine("Please enter description about file");
                  statusBarText.Text = "Description Can not be Empty";
                  return;
              }
            if (_filePath == "" && _fileName == null)
            {
                Console.WriteLine("Please provide File Path");
                statusBarText.Text = "File Path Can not be empty";
                return;
            }
              if (DepChk.IsChecked == true)
              {
                  msg.add("catogries", _catogries);
              }
            if (ChkInMode.IsChecked == false)
                msg.add("status", "open");
            else
                msg.add("status", "closed");
            if (translater != null)
            {
                msg.add("to", CsEndPoint.toString(serverEndPoint));
                msg.add("from", CsEndPoint.toString(endPoint_));
                msg.add("command", "check-in");
                msg.add("check-inMessage", "File Check-in Message");
                msg.add("author", _authorName);
                msg.add("description", desc);
                msg.add("dependencies", dependecies);
                msg.add("fileName", _fileName);
                msg.add("path", _filePath);
                translater.postMessage(msg);
               }
            else
                statusBarText.Text = "Please connect to Repository before sending check-in";
            afterCheckin(msg);
        }
        //----------------< Clears check-in button if selected wrong selection >----------------------------
        private void ChkInClearButton_Click(object sender, RoutedEventArgs e)
        {
            RepoFiles.Items.Clear();
            ChkInCatName.Text = "";
            ChkInDeps.Items.Clear();
            ChkInFile.Text = "";
            PkgDescript.Text = "";
            authorName.Text = "";
            initializeChkIn();
            statusBarText.Text = "Status: checkin canceled";
        }
        //-----------------------< Select category of check-in file >-------------------
        private void check_Selected(object sender, RoutedEventArgs e)
        {
            Dispatcher.BeginInvoke((Action)(() => {
                ChkInCatsButton_Click(null, null);
                statusBarText.Text = "Action: select checkin category by double clicking on category";
            }));
        }
        //-----------------< Select dependencies >-------------------
        private void ChkInDeps_MouseDoubleClick(object sender, RoutedEventArgs e)
        {
            object selection = ChkInDeps.SelectedItem;
            ChkInDeps.Items.Remove(selection);
            statusBarText.Text = "Status: Removed child relationship for next checkin";
        }
        /*----< return list of files in storage/category >-------------*/

        public List<String> filesFunc(String nameSpace, bool showPath = false)
        {
            String path = Path.Combine("../../../Storage",nameSpace);
            String[] files = Directory.GetFiles(path);
            for (int i = 0; i < files.Length; ++i)
            {
                if (showPath)
                    files[i] = Path.GetFullPath(files[i]);  // now an absolute FileSpec
                else
                    files[i] = Path.GetFileName(files[i]);  // now a FileName
            }
            return files.ToList<String>();
        }
        //--------------------< select the dependencies >------------
        private void RepoFiles_MouseDoubleClick(object sender, RoutedEventArgs e)
        {
             if (chkInDisplayMode == DisplayMode.nameSpace)
             {
                 if (ChkInCatName.Text.Length == 0)
                 {
                     ChkInCatName.Text = (string)RepoFiles.SelectedItem;
                     BrowseButton.IsEnabled = true;
                     Instructions.Content = "Second: browse for checkin file";
                     statusBarText.Text = "Action: browse for checkin file";
                 }
                 nameSpaceInfo = String.Copy((string)RepoFiles.SelectedItem);    
                 List<string> files = filesFunc(nameSpaceInfo);
                 RepoFiles.Items.Clear();
                 foreach (string file in files)
                 {
                         RepoFiles.Items.Add(file);      
                 }
                 chkInDisplayMode = DisplayMode.files;
                 ChkInCatName.Visibility = Visibility.Visible;
             }
             else
             {
                 // double click on file to add to dependency list
                 string fileName = (string)RepoFiles.SelectedItem;
                 string path = System.IO.Path.Combine(fileName);
                 if (!ChkInDeps.Items.Contains(path))
                     ChkInDeps.Items.Add(path);
             }
        }
        //-------------------------< Check-out message helper >---------------------------------
        private void formCheckoutMessage()
        {
            Console.WriteLine("\n   Requirement #3c check-out click button simulation from Check-out Tab \n============================================================");
            CsMessage msg = new CsMessage();
            if (translater != null)
            {

                msg.add("to", CsEndPoint.toString(serverEndPoint));
                msg.add("from", CsEndPoint.toString(endPoint_));
                msg.add("command", "check-out");
                if (ChkOutDesc.IsChecked == true)
                    msg.add("Dependents", "yes");
                else
                    msg.add("Dependents", "no");
                msg.add("messageText", "Sending a checkout request");
                msg.add("fileName", FilesInStorage.SelectedItem.ToString());
                translater.postMessage(msg);
                Console.WriteLine("      Check-out Request     \n-----------------------------------------");
                msg.show();
            }
            else
                statusBarText.Text = "Please connect to Repository before sending check-out click";
        }
        //-----------------------< Browse to ge testDrivers Information > -----------------------
        private void chkoutClick(object sender, RoutedEventArgs e)
        {
           
            formCheckoutMessage();

        }

        //------------------< To show all the files present in the Repository server >-----------
        private String[] getProjectDirsToPopulate()
        {
            string path = "";

            try
            {
                path = Path.GetFullPath("../../../Storage");
            }
            catch (Exception ex)
            {
                Console.WriteLine("Path Error Could not find path", ex);
            }
            string[] dirs = System.IO.Directory.GetDirectories(path);
            return dirs;
        }
        //-------------------< To find the dependency file info >--------------------------
        private void CheckBoxZone_Checked(object sender, RoutedEventArgs e)
        {
            CheckBox chk = (CheckBox)sender;
            int index = Int32.Parse(chk.Tag.ToString());
            depDict[index] = chk.Content.ToString();
        }
        //-----------------< To handle the dependencies if they are unchecked >-----------------------
        private void CheckBoxZone_UnChecked(object sender, RoutedEventArgs e)
        {
            CheckBox chk = (CheckBox)sender;
            int index = Int32.Parse(chk.Tag.ToString());
            depDict[index] = "";
        }
        //----------------< To get categories information >----------------------------------------
        private void Categories_Checked(object sender, RoutedEventArgs e)
        {
            CheckBox chk = (CheckBox)sender;
            int index = Int32.Parse(chk.Tag.ToString());
            catDict[index] = chk.Content.ToString();
        }
        //----------------< To handle information about categories unchecked >----------------------
        private void Categories_UnChecked(object sender, RoutedEventArgs e)
        {
            CheckBox chk = (CheckBox)sender;
            int index = Int32.Parse(chk.Tag.ToString());
            catDict[index] = "";
        }
        /*----< select file to checkin >-------------------------------*/
        /*
         *  If current version exists in Repository, show important
         *  parts of its metadata, so user doesn't have to specify
         *  unless it needs to change for this checkin.
         */
        private void BrowseButton_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog fileDlg = new OpenFileDialog();
            if (fileDlg.ShowDialog() == true)
            {
                _fileName = fileDlg.FileName;
                _filePath = Path.GetFullPath(fileDlg.FileName);
                ChkInFile.Text = System.IO.Path.GetFileName(_fileName);
                ChkInButton.IsEnabled = true;
                ChkInDeps.Items.Clear();
                BrowseButton.IsEnabled = false;
                Instructions.Content = "Third: add child by dbl clk on file";
                statusBarText.Text = "Action: optionally add child by double clicking on file, then click CheckIn button";
            }
        }

        //-------------------------< Populate the categories list of datatemplate >---------
        public void populateCategories()
        {
            
            TheCat.Clear();
            TheCat.Add(new BoolStringClass { TheText = "Display", TheValue = 1 });
            TheCat.Add(new BoolStringClass { TheText = "Test", TheValue = 2 });
            TheCat.Add(new BoolStringClass { TheText = "GUI", TheValue = 3 });

            this.DataContext = this;
        }
        //----------------------------< Populate the data template >---------------------
        public void populatedStatus()
        {
            TheStat = new ObservableCollection<BoolStringClass>();
            TheStat.Add(new BoolStringClass { TheText = "Close", TheValue = 1 });
            TheStat.Add(new BoolStringClass { TheText = "Open", TheValue = 2 });
            this.DataContext = this;
        }
        //----< strip off name of first part of path >---------------------
        private string removeFirstDir(string path)
        {
            string modifiedPath = path;
            int pos = path.IndexOf("/");
            modifiedPath = path.Substring(pos + 1, path.Length - pos - 1);
            return modifiedPath;
        }

        //---------------------< Add tests to the build request >--------------------------------
        private void DirList_MouseDoubleClick(object sender, RoutedEventArgs e)
        {
            
            string selectedDir = (string)DirList.SelectedItem;
            string path;
            if (selectedDir == "..")
            {
                if (pathStack_.Count > 1)  // don't pop off "Storage"
                    pathStack_.Pop();
                else
                    return;
            }
            else
            {
                path = pathStack_.Peek() + "/" + selectedDir;
                pathStack_.Push(path);
            }
            
            PathTextBlock.Text = removeFirstDir(pathStack_.Peek());

            CsMessage msg = new CsMessage();
            if (translater != null)
            {
                msg.add("to", CsEndPoint.toString(serverEndPoint));
                msg.add("from", CsEndPoint.toString(endPoint_));
                msg.add("command", "getDirs");
                msg.add("path", pathStack_.Peek());
                Console.WriteLine("     GetDirs Request Part of Browse\n=================================");
                msg.show();
                translater.postMessage(msg);
                msg.remove("command");
                msg.add("command", "getFiles");
                Console.WriteLine("     GetFiles Request Part of Browse\n=================================");
                msg.show();
                translater.postMessage(msg);
            }
        }
        private void fileContentHelper(string fileName)
        {
            CsMessage msg = new CsMessage();
            if (translater != null)
            {
                msg.add("to", CsEndPoint.toString(serverEndPoint));
                msg.add("from", CsEndPoint.toString(endPoint_));
                msg.add("command", "fileContent");
                msg.add("fileName", fileName);
                translater.postMessage(msg);
                Server_Message.Items.Add("File content View Message from:" + msg.value("from") + " to:" + msg.value("to"));
            }
            else
                statusBarText.Text = "Please Set up Connection Before Browse";
        }

        //----------------------< Popups a window to show the content of the file >--------------------
        private void fileContentView(object sender, RoutedEventArgs e)
        {
            string fileName = (string)FileList.SelectedItem;
            fileContentHelper(fileName);
        }
        //--------------------------< shows the content of file >--------------------------------------
        public void showFileContent(string fileName)
        {
            Console.WriteLine("\n  CodePopup   Requirement #3 \n==========================================");
            List<string> helper2 = new List<string>(fileName.Split('@'));
            string fileName1 = helper2[0];
            string path = Path.GetFullPath(fileName1);
            CodePopUp popup = new CodePopUp();
            try
            {
                
                Paragraph paragraph = new Paragraph();
                Paragraph paragraph2 = new Paragraph();
                string fileText = "";
                string metadata = "";
                try
                {
                    fileText = System.IO.File.ReadAllText(path);
                    if (helper2.Count() == 2)
                        metadata = helper2[1];
                }
                finally
                {
                    paragraph.Inlines.Add(new Run(fileText));
                    paragraph2.Inlines.Add(new Run(metadata));
                }
                popup.metaDataView.Blocks.Clear();
                popup.metaDataView.Blocks.Add(paragraph2);

                popup.codeView.Blocks.Clear();
                popup.codeView.Blocks.Add(paragraph);
                popup.Show();
              
            }
            catch (Exception ex)
            {
                string msg = ex.Message;
                Console.WriteLine("Error{0}", msg);
            }
            return;
        }
     

        //-----------------------------< the config files that shows configuration file details >------------
        private void BrowseSetup(object sender, RoutedEventArgs e)
        {
            PathTextBlock.Text = "Storage";
            pathStack_.Push("../Storage");
            CsMessage msg = new CsMessage();
            if (translater != null)
            {
                msg.add("to", CsEndPoint.toString(serverEndPoint));
                msg.add("from", CsEndPoint.toString(endPoint_));
                msg.add("command", "getDirs");
                msg.add("path", pathStack_.Peek());
                translater.postMessage(msg);
                Server_Message.Items.Add("GetDirs Request as part of Browse from:" + msg.value("from") + " to:" + msg.value("to"));
                msg.remove("command");
                msg.add("command", "getFiles");
                translater.postMessage(msg);
                Server_Message.Items.Add("GetFiles Request as part of Browse from:" + msg.value("from") + " to:" + msg.value("to"));
            }
            else
                statusBarText.Text = "Please Set up Connection Before Browse";
        }
        private void addFile(string fileName)
        {
            FilesInStorage.Items.Add(fileName);
        }
        private IEnumerable<string> GetFiles(string path)
        {
            Queue<string> queue = new Queue<string>();
            queue.Enqueue(path);
            while (queue.Count > 0)
            {
                path = queue.Dequeue();
                try
                {
                    foreach (string subDir in Directory.GetDirectories(path))
                    {
                        queue.Enqueue(subDir);
                    }
                }
                catch (Exception ex)
                {
                    Console.Error.WriteLine(ex);
                }
                string[] files = null;
                try
                {
                    files = Directory.GetFiles(path);
                }
                catch (Exception ex)
                {
                    Console.Error.WriteLine(ex);
                }
                if (files != null)
                {
                    for (int i = 0; i < files.Length; i++)
                    {
                        yield return files[i];
                    }
                }
            }
        }
        //---------------< Load Files in storage to check-out >---------------
        private void loadStoraeFiles(object s, RoutedEventArgs e)
        {

            FilesInStorage.Items.Clear();
            IEnumerable<string> files = GetFiles("../../../Storage");
            foreach (string file in files)
            {
                if (Dispatcher.CheckAccess())
                    FilesInStorage.Items.Add(Path.GetFileName(file));
                else
                {
                    Action<string> _addFile = (string fname) =>
                    {
                        addFile(fname);
                    };
                    Dispatcher.Invoke(_addFile, new Object[] { Path.GetFileName(file) });
                }
            }

            FilesInStorage.SelectedIndex = 0;
        }
        //--------------< To simulate the browser file double click >---------------------
        private void testBrosweDoubleClick()
        {
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getDirs");
            msg.add("path", "../Storage/NosqlDb");
            translater.postMessage(msg);
            msg.remove("command");
            msg.add("command", "getFiles");
            translater.postMessage(msg);
            return;
        }
        //-------------------< Check-in >-------
        private void testCheckIn1(object sender,RoutedEventArgs e)
        {
            ChkInDeps.Items.Clear();
            Console.WriteLine("\n Check-in without chaining Version \"Editing File already Present\"\n---------------------------------------------------");
            authorName.Text = "Harika";
            PkgDescript.Text = "Second File to check-in";
            DepChk.IsChecked = true;
            ChkInMode.IsChecked = false;
            catDict[0] = "Build";
            catDict[1] = "Test";
            ChkInDeps.Items.Add("NosqlDb.DBCore.cpp.1");
            _fileName = "NosqlDb.Queries.cpp";
            _filePath = Path.GetFullPath("../../../Stagging");
            ChkInButton_Click(sender, e);
            return;
        }
        //--------------------< To simulate the check-in case for editing the file without changing the version >-------------------
        private void testcheckin(object sender, RoutedEventArgs e)
        {
            ChkInDeps.Items.Clear();
            authorName.Text = "Harika";
            PkgDescript.Text = "First File to check-in";
            DepChk.IsChecked = true;
            ChkInMode.IsChecked = false;
            catDict[0] = "Build";
            catDict[1] = "Test";
            ChkInDeps.Items.Add("NosqlDb.Queries.h.1");
            _fileName = "NosqlDb.DBCore.cpp";
            _filePath = Path.GetFullPath("../../../Stagging");
            ChkInButton_Click(sender, e);
            return;
        }
        //--------------------< To simulate the check-in click programatically >-------------------
        private void testcheckinClose(object sender, RoutedEventArgs e)
        {
            ChkInDeps.Items.Clear();
            authorName.Text = "Harika";
            PkgDescript.Text = "First File to check-in";
            DepChk.IsChecked = true;
            catDict[0] = "Build";
            catDict[1] = "Test";
            ChkInDeps.Items.Add("NosqlDb.Queries.h.1");
            _fileName = "NosqlDb.DBCore.cpp";
            _filePath = Path.GetFullPath("../../../Stagging");
            ChkInMode.IsChecked = true;
            ChkInButton_Click(sender, e);
            return;
        }

        //--------------------< To simulate the check-in click programatically >-------------------
        private void testCheckinClose2(object sender, RoutedEventArgs e)
        {
            ChkInDeps.Items.Clear();
            authorName.Text = "Harika";
            PkgDescript.Text = "Second File to check-in";
            DepChk.IsChecked = true;
            ChkInMode.IsChecked = true;
            catDict[0] = "Build";
            catDict[1] = "Test";
            ChkInDeps.Items.Add("NosqlDb.DBCore.cpp.1");
            _fileName = "NosqlDb.Queries.cpp";
            _filePath = Path.GetFullPath("../../../Stagging");
            ChkInButton_Click(sender, e);
            return;
        }
        //--------------------< To simulate the check-in click programatically >-------------------
        private void testCheckinClose3(object sender, RoutedEventArgs e)
        {
            ChkInDeps.Items.Clear();
            authorName.Text = "Harika";
            PkgDescript.Text = "Second File to check-in";
            DepChk.IsChecked = true;
            ChkInMode.IsChecked = true;
            catDict[0] = "Build";
            catDict[1] = "Test";
            ChkInDeps.Items.Add("NosqlDb.Queries.cpp.1");
            _fileName = "NosqlDb.Queries.h";
            _filePath = Path.GetFullPath("../../../Stagging");
            ChkInButton_Click(sender, e);
            return;
        }
        //-------------------< Query result to view file double click >-------------------
        private void queryResult_MouseDoubleClick(object sender,RoutedEventArgs e)
        {
            string fileName = (string)queryResults.SelectedItem;
            List<string> helper = new List<string>(fileName.Split('.'));
            List<string> helper2 = new List<string>(fileName.Split(':'));
            string fileNameFinal = Path.Combine("../../../Storage/" + helper[0], helper2[0]);
            showFileContent(fileNameFinal);
        }
        //---------------------< Preapare custom search >--------------
        private void formCustomSearchQuery()
        {
            CsMessage msg = new CsMessage();
            if (translater != null)
            {

                msg.add("to", CsEndPoint.toString(serverEndPoint));
                msg.add("from", CsEndPoint.toString(endPoint_));
                string commd = QueryType.Text.ToString();
                string regExpr = RegularExpression.Text;
                msg.add("command", "customQuery");
                msg.add("type", commd);
                if (regExpr != null)
                    msg.add("regularExpres", regExpr);
                translater.postMessage(msg);
                Server_Message.Items.Add("Custom Query Request from:" + msg.value("from") + " to:" + msg.value("to"));
                Console.Write("\n      customQuery    \n-----------------------------------------");
                msg.show();
            }
            else
                statusBarText.Text = "Please connect to Repository before sending check-out click";

        }
        //-----------------< custom Search >---------------
        private void CustomSearch(object sender,RoutedEventArgs e)
        {
            formCustomSearchQuery();
        }
        //--------------------< To simulate the check-in for creating a new version>-------------------
        private void testcheckinNewVersion(object sender, RoutedEventArgs e)
        {
            Console.WriteLine("\n Check-in New Version as previous version is closed\n----------------------------------------");
            ChkInDeps.Items.Clear();
            authorName.Text = "Harika";
            PkgDescript.Text = "New Version Check-in ";
            DepChk.IsChecked = true;
            catDict[0] = "Build";
            catDict[1] = "Test";
            ChkInDeps.Items.Add("NosqlDb.Queries.h.1");
            _fileName = "NosqlDb.DBCore.cpp";
            _filePath = Path.GetFullPath("../../../Stagging");
            ChkInMode.IsChecked = false;
            ChkInButton_Click(sender, e);
            return;
        }
        //--------------------< To simulate the check-in case for editing the file without changing the version >-------------------
        private void testcheckinSample(object sender, RoutedEventArgs e)
        {
            ChkInDeps.Items.Clear();
            authorName.Text = "Harika";
            PkgDescript.Text = "First File to check-in";
            DepChk.IsChecked = true;
            ChkInMode.IsChecked = false;
            catDict[0] = "Build";
            catDict[1] = "Test";
            _fileName = "test.testFile1.cs";
            _filePath = Path.GetFullPath("../../../Stagging");
            ChkInButton_Click(sender, e);
            return;
        }

        //--------------------< To simulate the check-in case for editing the file without changing the version >-------------------
        private void testcheckinSample2(object sender, RoutedEventArgs e)
        {
            ChkInDeps.Items.Clear();
            Console.WriteLine("\n Check-in Fails : Closing Parent Without Closing Dependents\n-------------------------------------------");
            authorName.Text = "Harika";
            PkgDescript.Text = "Close to check-in";
            DepChk.IsChecked = true;
            ChkInMode.IsChecked = true;
            catDict[0] = "Build";
            catDict[1] = "Test";
            ChkInDeps.Items.Add("test.testFile1.cs.1");
            _fileName = "NosqlDb.Persist.cpp";
            _filePath = Path.GetFullPath("../../../Stagging");
            ChkInButton_Click(sender, e);
            return;
        }

        //-------------------< To simulate the check-in fail case for violating ownership policy>----------
        private void testCheckinFailCase1(object sender,RoutedEventArgs e)
        {
            ChkInDeps.Items.Clear();
            Console.WriteLine("\n Check-in Fails : Violation of Single Owner Policy\n-------------------------------------------");
            authorName.Text = "Bandaru";
            PkgDescript.Text = "First File to check-in";
            DepChk.IsChecked = true;
            catDict[0] = "Build";
            catDict[1] = "Test";
            ChkInDeps.Items.Add("NosqlDb.Queries.h.1");
            _fileName = "NosqlDb.DBCore.cpp";
            _filePath = Path.GetFullPath("../../../Stagging");
            ChkInMode.IsChecked = false;
            ChkInButton_Click(sender, e);
            return;

        }
        //------------------< To show database >------------------------
        private void showDataBase(object sender, RoutedEventArgs e)
        {
            CsMessage msg = new CsMessage();
            if (translater != null)
            {
                msg.add("to", CsEndPoint.toString(serverEndPoint));
                msg.add("from", CsEndPoint.toString(endPoint_));
                msg.add("command", "Display");
                translater.postMessage(msg);
            }
        }
      
        //------------< To simulate check-out with out dependencies >----------
        private void testCheckoutWithOutDep()
        {
            Console.WriteLine("\n Check-out without dependencies \n-------------------------------------------");
            ChkOutDesc.IsChecked = false;
            FilesInStorage.SelectedItem = FilesInStorage.Items.GetItemAt(1);
            Server_Message.Items.Add("Check-out without Dep Message from client to repo: "+ FilesInStorage.SelectedItem.ToString());
            formCheckoutMessage();
        }
        //------------< To simulate check-out with Dependencies >-------------
        private void testCheckoutWithDep()
        {
            ChkOutDesc.IsChecked = true;
            CsMessage msg = new CsMessage();
            if (translater != null)
            {

                msg.add("to", CsEndPoint.toString(serverEndPoint));
                msg.add("from", CsEndPoint.toString(endPoint_));
                msg.add("command", "check-out");
                if (ChkOutDesc.IsChecked == true)
                    msg.add("Dependents", "yes");
                else
                    msg.add("Dependents", "no");
                msg.add("messageText", "Sending a checkout request");
                msg.add("fileName","NosqlDb.Queries.h.1");
                Server_Message.Items.Add("Check-out with Dep Message from client to repo: NosqlDb.Queries.h.1");
                translater.postMessage(msg);
                Console.WriteLine("\n     Check-out With Dependencies See Check-out Tab \n-----------------------------------------");
                Console.WriteLine("\n For better view see the Client GUI #2 check-out tab");
                msg.show();
            }
            else
                statusBarText.Text = "Please connect to Repository before sending check-out click";
        }
        //----------------< Custom Query test using the Query package designed in Project #1
        private void testCustomQueries()
        {
            Console.WriteLine("\nCustom Queries Request for Categories \"Xml\"\n--------------------------------------");
            QueryType.SelectedIndex = 0;
            RegularExpression.Text = "Xml";
            formCustomSearchQuery();
        }
        //---------------------< Form custom queries >------------------------------
        private void testCustomQueries2()
        {
            Console.WriteLine("\nCustom Queries Request for Categories \"FileName\"\n--------------------------------------");
            QueryType.SelectedIndex = 1;
            RegularExpression.Text = "NosqlDb.Queries.h.1";
            formCustomSearchQuery();
        }
        //----------------< Form custom queries for unit-test >--------------
        private void testCustomQueries3()
        {
            Console.WriteLine("\nCustom Queries Request for Categories \"Version\"\n--------------------------------------");
            QueryType.SelectedIndex = 4;
            RegularExpression.Text = "1";
            formCustomSearchQuery();
        }
        //--------------< Form custom queries for unit - test >---------------
        private void testCustomQueries4()
        {
            Console.WriteLine("\nCustom Queries Request for Categories \"Status:Closed\"\n--------------------------------------");
            QueryType.SelectedIndex = 3;
            RegularExpression.Text = "closed";
            formCustomSearchQuery();
        }
        //--------------< Display the File Content >--------------
        private void FileContentDisplay()
        {
            fileContentHelper("NosqlDb.Persist.cpp.1");    
        }
       
    }

}
