/////////////////////////////////////////////////////////////////////
// MainWindow.xaml.cs - Prototype for OOD Project #4               //
// ver 1.1                                                         //
// Jim Fawcett, CSE687 - Object Oriented Design, Fall 2018         //
/////////////////////////////////////////////////////////////////////
/*
 * Package Operations:
 * -------------------
 * This package defines one class MainWindow that provides tabs:
 * - Find Libs: Navigate through local directory to find files for testing
 *   - Shows local directories and files
 *   - Can navigate by double clicking directories
 *   - Can select files by selecting.  That displays selections in a popup window.
 * - Request Tests: left for students
 * - Show Results:  left for students
 * 
 * Required Files:
 * ---------------
 * MainWindow.xaml, MainWindow.xaml.cs
 * SelectionWindow.xaml, SelectionWindow.xaml.cs
 * 
 * Maintenance History:
 * --------------------
 * ver 1.1 : 16 Nov 2018
 * - fixed bug in Files_SelectionChanged by checking e.AddedItems.Count
 *   and returning if 0.
 * ver 1.0 : 15 Nov 2018
 * - first release
 * 
 */
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Threading;
using System.IO;
using System.Diagnostics;
using MsgPassingCommunication;


namespace GUI
{
  ///////////////////////////////////////////////////////////////////
  // MainWindow class
  // - Provides navigation and selection to find libraries to test.
  // - Creates a popup window to handle selections.
  
   
  public partial class MainWindow : Window
  {
    public string SavePath { get; set; }
    public string SendPath { get; set; }
    public List<string> selectedFiles { get; set; } = new List<string>();
    public SelectionWindow swin { get; set; } = null;
    bool unselecting = false;
    
    public CsMessage msg;
    private Stack<string> pathStack_ = new Stack<string>();
    private Translater translater;
    private Thread recv = null;
    private CsEndPoint from_;
    private CsEndPoint to_;
    private string to_string;
    private string from_string;
    private Dictionary<string, Action<CsMessage>> dispatcher_
   = new Dictionary<string, Action<CsMessage>>();

//----< Default constructor for Main window >-------
        public MainWindow()
        {
            InitializeComponent();
            start();
            processMessages();
            update_status("Welcome! GUI@" + from_string + " TestHarness@" + to_string + "...");
            setup_dispatcher();
        }

//----< Start the Communication part of Gui >-------
        private void start()
        {
            msg = new CsMessage();
            from_ = new CsEndPoint();
            from_.machineAddress = "localhost";
            from_.port = 8099;//
            to_ = new CsEndPoint();
            to_.machineAddress = "localhost";
            to_.port = 8888;//To TestHarness
            to_string = to_.machineAddress + ":" + to_.port;
            from_string = from_.machineAddress + ":" + from_.port;
            translater = new Translater();
            translater.listen(from_);//Start Comm        
        }

 //----< Setup action when result message received >-------
        private void setup_dispatcher()
        {
            addClientProc("result",handle_result);
        }


        //----< Process Messages Received >-------
        private void processMessages()
        {
            ThreadStart thrdProc = () => {
                while (true)
                {
                    CsMessage msg = translater.getMessage();
                    if (msg.contain_key("command"))
                    {
                        string msgId = msg.value("command");
                        if (dispatcher_.ContainsKey(msgId))
                            dispatcher_[msgId].Invoke(msg);
                        else
                            update_status("Invalid message!");
                    }

                }
            };
            recv = new Thread(thrdProc);
            recv.IsBackground = true;
            recv.Start();
        }

        //----< load all dispatcher processing >---------------------------

        private void loadDispatcher()
        {
            DispatcherLoadGetDirs();
            DispatcherLoadGetFiles();
        }

        //----< function dispatched by child thread to main thread >-------

        private void clearDirs()
        {
            //DirList.Items.Clear();
        }

        //----< function dispatched by child thread to main thread >-------

        private void addDir(string dir)
        {
            //DirList.Items.Add(dir);
        }

        //----< load getDirs processing into dispatcher dictionary >-------

        private void DispatcherLoadGetDirs()
        {
            Action<CsMessage> getDirs = (CsMessage rcvMsg) =>
            {
                Action clrDirs = () =>
                {
                    clearDirs();
                };
                Dispatcher.Invoke(clrDirs, new Object[] { });
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
                Dispatcher.Invoke(insertUp, new Object[] { });
            };
            addClientProc("getDirs", getDirs);
        }

        //----< function dispatched by child thread to main thread >-------

        private void insertParent()
        {
            Dirs.Items.Insert(0, "..");
        }

        //----< function dispatched by child thread to main thread >-------

        private void clearFiles()
        {
            Files.Items.Clear();
        }

        //----< function dispatched by child thread to main thread >-------

        private void addFile(string file)
        {
            Files.Items.Add(file);
        }

        //----< add client processing for message with key >---------------

        private void addClientProc(string key, Action<CsMessage> clientProc)
        {
            dispatcher_[key] = clientProc;
        }

        //----< load getFiles processing into dispatcher dictionary >------

        private void DispatcherLoadGetFiles()
        {
            Action<CsMessage> getFiles = (CsMessage rcvMsg) =>
            {
                Action clrFiles = () =>
                {
                    clearFiles();
                };
                Dispatcher.Invoke(clrFiles, new Object[] { });//What?
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("sendingFile"))
                    {
                        Action<string> doFile = (string file) =>
                        {
                            addFile(file);
                        };
                        Dispatcher.Invoke(doFile, new Object[] { enumer.Current.Value });
                    }

                    if (key.Contains("sendingLog"))
                    {
                        Action<string> doLog = (string file) =>
                        {
                            addFile(file);
                        };
                        Dispatcher.Invoke(doLog, new Object[] { enumer.Current.Value });
                    }
                }
            };
            addClientProc("getFiles", getFiles);
        }

        //----< open window showing contents of project directory >------

        private void Window_Loaded(object sender, RoutedEventArgs e)
    {
      //Default library: ../SendFile
      string curpath = Directory.GetCurrentDirectory();
      SendPath = getAncestorPath(1, curpath) + "/SendFile";
      SavePath = getAncestorPath(1, curpath) + "/SaveFile";
      LoadNavTab(SendPath);
    }
        //----< find parent paths >--------------------------------------

        string getAncestorPath(int n, string path)
    {
      for(int i=0; i<n; ++i)
        path = Directory.GetParent(path).FullName;
      return path;
    }
    //----< file Find Libs tab with subdirectories and files >-------

    private void LoadNavTab(string path)
    {
      Dirs.Items.Clear();
      CurrPath.Text = path;
      string[] dirs = Directory.GetDirectories(path);
      Dirs.Items.Add("..");
      foreach (string dir in dirs)
      {
        DirectoryInfo di = new DirectoryInfo(dir);
        string name = System.IO.Path.GetDirectoryName(dir);
        Dirs.Items.Add(di.Name);
      }
      Files.Items.Clear();
      string[] files = Directory.GetFiles(path);
      foreach (string file in files)
      {
        string name = System.IO.Path.GetFileName(file);
        Files.Items.Add(name);
      }
    }
    //----< handle selections in files listbox >---------------------

    private void Files_SelectionChanged(object sender, SelectionChangedEventArgs e)
    {
      if(unselecting)
      {
        unselecting = false;
        return;
      }

      if(swin == null)
      {
        swin = new SelectionWindow();
        swin.setMainWindow(this);
      }
        swin.Show();

      if (e.AddedItems.Count == 0) return;

      string selStr = e.AddedItems[0].ToString();
      selStr = System.IO.Path.Combine(SendPath, selStr);
      if (!selectedFiles.Contains(selStr))
      {
        selectedFiles.Add(selStr);
        swin.Add(selStr);
      }
    }
    //----< unselect files called by unloading SelectionWindow >-----

    public void unselectFiles()
    {
      unselecting = true;  // needed to avoid using selection logic
      Files.UnselectAll();
    }
    //----< move into double-clicked directory, display contents >---

    private void Dirs_MouseDoubleClick(object sender, MouseButtonEventArgs e)
    {
      if (Dirs.SelectedItem != null) {
      string selectedDir = Dirs.SelectedItem.ToString();
      if (selectedDir == "..")
        SendPath = getAncestorPath(1, SendPath);
      else
        SendPath = System.IO.Path.Combine(SendPath, selectedDir);
      LoadNavTab(SendPath);
      }
    }
    //----< shut down the SelectionWindow if open >------------------

    private void Window_Unloaded(object sender, RoutedEventArgs e)
    {
      swin.Close();
    }

    //----< Receive message>------------------

     private CsMessage getMsgs()
    {
        CsMessage msg = new CsMessage();
        msg = translater.getMessage();
        return msg;
    }

//----< Send message to Testharness server >------------------

        private void Send_Click(object sender, RoutedEventArgs e)
    {
         msg.add("to", to_string);
         msg.add("from", from_string);
         msg.add("name", tq_name.Text.ToString());msg.add("author", tq_author.Text.ToString());
         msg.add("command", "testReq");
      
        update_status("Sending test request to Test Harness server @" + to_string);
        if (translater.postMessage(msg)) update_status("Sending Request done!");
    }
       
//----< Clear current message and all fiels with content>------------------

        private void Clear_Click(object sender, RoutedEventArgs e)
    {
            new Thread(() =>
            {
                this.Dispatcher.Invoke(new Action(() =>
                {
                    tq_name.Text = "";
                    tq_author.Text = "";
                    show_dlls.Items.Clear();
                }));
            }).Start();

            selectedFiles.Clear();
            update_status("Clear selected libraries done!");
    }

//----<  >------------------

        private void Result_MouseDoubleClick(object sender, RoutedEventArgs e)
    {
            new Thread(() =>
            {
                this.Dispatcher.Invoke(new Action(() =>
                {
                    //result_detail.Items.
                }));
            }).Start();
    }
        //----< Update status bar >------------------

        private void update_status(string text)
        {
            new Thread(() =>
            {
                this.Dispatcher.Invoke(new Action(() =>
                {
                    status.Text = text;
                }));
            }).Start();

        }

        //----< Update test result >------------------

        private void handle_result(CsMessage msg)
        {

            new Thread(() =>
            {
                this.Dispatcher.Invoke(new Action(() =>
                {   //Display test results in GUI
                    string result_info = "";
                    result_info = "Name: ";
                    if (msg.contain_key("name")) { result_info += msg.value("name"); }
                    result_info += "\nAuthor: ";
                    if (msg.contain_key("author")) { result_info += msg.value("author"); }
                    result_info += "\nTime: ";
                    if (msg.contain_key("time")) { result_info += msg.value("time"); }
                    result_info += "\nLibrary: ";
                    if (msg.contain_key("dll")) { result_info += msg.value("dll"); }
                    result_info += "\nResult: ";
                    if (msg.contain_key("result")) { result_info += msg.value("result"); }
                    result.Items.Add(result_info);
                }));
            }).Start();
        }

    }
    }

