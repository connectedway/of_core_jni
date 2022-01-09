import java.awt.*;
import java.awt.image.BufferedImage ;
import java.awt.event.*;
import java.util.*;
import javax.swing.*;
import javax.swing.tree.*;
import javax.swing.event.*;
import javax.imageio.ImageIO;
import java.lang.Thread ;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.FileNotFoundException;

import com.connectedway.io.*;

public class OfcExplorer
    extends JFrame 
{
    /**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	public static final ImageIcon ICON_COMPUTER = 
	new ImageIcon("computer.gif");
    public static final ImageIcon ICON_DISK = 
	new ImageIcon("disk.gif");
    public static final ImageIcon ICON_FOLDER = 
	new ImageIcon("folder.gif");
    public static final ImageIcon ICON_EXPANDEDFOLDER = 
	new ImageIcon("expandedfolder.gif");

    protected JTree  m_tree;
    protected DefaultTreeModel m_model;
    protected JTextField m_display;

    protected JPopupMenu m_popup;
    protected Action m_action;
    protected TreePath m_clickedPath;
    protected TreePath m_clipboardPath;

    protected enum clipboardAction {
	clipboardCut, clipboardCopy, clipboardEmpty } 
    protected clipboardAction m_clipboardAction ;

    protected Framework framework ;

    public class StreamGobbler extends Thread {
	InputStream is;

	StreamGobbler(InputStream is) {
	    this.is = is ;
	}

	public void run() {
	    try {
		InputStreamReader isr = new InputStreamReader(is);
		BufferedReader br = new BufferedReader(isr) ;
		String line = null ;
		while ((line = br.readLine()) != null)
		    System.out.println (line);
	    } catch (IOException ioe) {
		ioe.printStackTrace() ;
	    }
	}
    }

    public class FileStreamer extends Thread {
	OutputStream os;
	File f ;

	FileStreamer(OutputStream os, File f) {
	    this.os = os ;
	    this.f = f ;
	}

	public void run() {
	    try {
		int count ;
		byte[] b = new byte[4096] ;
		FileInputStream fis = new FileInputStream (f) ;

		while ((count = fis.read(b,0,4096)) != -1)
		    os.write(b, 0, count) ;

		fis.close() ;
		os.close() ;
	    } catch (IOException ioe) {
		ioe.printStackTrace() ;
	    }
	}
    }

    public OfcExplorer(String config_file)
    {
	super("Open Files Java Explorer");
	setSize(400, 300);

	DefaultMutableTreeNode top = 
	    new DefaultMutableTreeNode(new IconData(ICON_COMPUTER, 
						    null, "Computer"));

	DefaultMutableTreeNode node;

	framework = Framework.getFramework() ;
	framework.load(new File (config_file)) ;
	framework.startup() ;

	File[] roots = File.listRoots();
	for (int k=0; k<roots.length; k++) {
	    node = new DefaultMutableTreeNode
		(new IconData(ICON_DISK, null, new FileNode(roots[k])));
	    top.add(node);
	    node.add(new DefaultMutableTreeNode( new Boolean(true) ));
	}

	m_model = new DefaultTreeModel(top);
	m_tree = new JTree(m_model);
	m_clipboardAction = clipboardAction.clipboardEmpty ;

	m_tree.putClientProperty("JTree.lineStyle", "Angled");

	TreeCellRenderer renderer = new 
	    IconCellRenderer();
	m_tree.setCellRenderer(renderer);

	m_tree.addTreeExpansionListener(new 
					DirExpansionListener());

	m_tree.addTreeSelectionListener(new 
					DirSelectionListener());

	//    JTextField field = new BlueTextField () ;

	//    DefaultTreeCellEditor editor = 
	//   	new DefaultTreeCellEditor(m_tree, 
	//    				  new DefaultTreeCellRenderer(),
	//    				  new DefaultCellEditor(field)) ;
	//    m_tree.setCellEditor(editor) ;

	m_tree.getModel().addTreeModelListener(new NodeModelListener()) ;

	m_tree.getSelectionModel().setSelectionMode
	    (TreeSelectionModel.SINGLE_TREE_SELECTION); 
	m_tree.setShowsRootHandles(true); 
	m_tree.setEditable(false);

	JScrollPane s = new JScrollPane();
	s.getViewport().add(m_tree);
	getContentPane().add(s, BorderLayout.CENTER);

	m_display = new JTextField();
	m_display.setEditable(false);
	getContentPane().add(m_display, BorderLayout.NORTH);

	m_popup = new JPopupMenu(); 

	m_action = new AbstractAction() {
		/**
		 * 
		 */
		private static final long serialVersionUID = 1L;

		public void actionPerformed(ActionEvent e) {
		    if (m_clickedPath==null)
			return;
		    if (m_tree.isExpanded(m_clickedPath))
			m_tree.collapsePath(m_clickedPath);
		    else
			{
			    m_tree.fireTreeExpanded(m_clickedPath) ;
			    // m_tree.expandPath(m_clickedPath);
			}
		}
	    };
	m_popup.add(m_action);

	m_popup.addSeparator();

	Action a0 = new AbstractAction("Open") {
		/**
		 * 
		 */
		private static final long serialVersionUID = 1L;

		@SuppressWarnings("unused")
		public void actionPerformed(ActionEvent e) {

		    TreePath path = m_tree.getSelectionPath() ;
		    DefaultMutableTreeNode node = null ;
		    FileNode fnode = null ;

		    m_clipboardAction = clipboardAction.clipboardEmpty ;
		    if (path != null) 
			node = getTreeNode(path) ;
		    if (node != null)
			fnode = getFileNode(node) ;

		    if (fnode != null) {
			File f = fnode.getFile() ;

			String name = f.getName().toUpperCase() ;
			if (name.endsWith(".JPEG") || name.endsWith(".JPG")) {
			    JFrame frame = new JFrame(f.getName()) ;
			    Panel panel = new Jpeg(f) ;
			    frame.getContentPane().add(panel) ;
			    frame.setSize(Toolkit.getDefaultToolkit().getScreenSize()) ;
			    frame.setVisible(true) ;
			} else if (name.endsWith(".MP3") || 
				   name.endsWith(".M4A") ||
				   name.endsWith(".M4V") ||
				   name.endsWith(".MP4") ||
				   name.endsWith(".FLV")) {
			    try {


				String[] commands = new String[] {
				    "/usr/bin/mplayer", "-cache", "1024", "-" } ;
				System.out.println ("Running mplayer") ;
				Process process = 
				    Runtime.getRuntime().exec (commands) ;
				try {
				    StreamGobbler errorGobbler = 
					new StreamGobbler(process.getErrorStream()) ;
				    StreamGobbler outputGobbler = 
					new StreamGobbler(process.getInputStream()) ;
				    FileStreamer fileStreamer =
					new FileStreamer(process.getOutputStream(), f) ;
				    errorGobbler.start() ;
				    outputGobbler.start() ;
				    fileStreamer.start() ;
				    process.waitFor() ;
				} catch (InterruptedException ex) {
				}
			    } catch (IOException ex) {
				System.out.println ("runtime exception") ;
			    }

			} else {
			    Editor ed = new Editor(f) ;
			}
		    }
		}
	    };
	m_popup.add(a0);

	m_popup.addSeparator();

	Action a01 = new AbstractAction("New Folder") {
		/**
		 * 
		 */
		private static final long serialVersionUID = 1L;

		public void actionPerformed(ActionEvent e) {

		    TreePath path = m_tree.getSelectionPath() ;
		    DefaultMutableTreeNode node = null ;
		    FileNode fnode = null ;

		    m_clipboardAction = clipboardAction.clipboardEmpty ;
		    if (path != null) 
			node = getTreeNode(path) ;
		    if (node != null)
			fnode = getFileNode(node) ;

		    if (fnode != null) {
			File f = fnode.getFile() ;

			if (!f.isDirectory()) {

			    if (f instanceof File) {
				long lastError = ((File)f).getLastError() ;
				if (lastError != 0)
				    System.out.println ("Can't Determine Directory, " +
							"Last Error " + lastError) ;
			    }

			    f = f.getParentFile() ;
			    node = (DefaultMutableTreeNode) 
				node.getParent() ;
			}

			String name = "New Folder" ;
			File f2 = new File (f, name) ;

			if (!f2.mkdir())
			    JOptionPane.showConfirmDialog
				(null, OfcExplorer.this,
				 "Unable to Create Directory",
				 JOptionPane.OK_OPTION) ;
			else {
			    DefaultMutableTreeNode newNode = 
				new DefaultMutableTreeNode (name) ;

			    newNode.setUserObject (new FileNode(f2)) ;
			
			    m_model.insertNodeInto (newNode, node,
						    node.getChildCount()) ;
			}
		    }
		}
	    };
	m_popup.add(a01);

	Action a01x = new AbstractAction("Authenticate") {
		/**
		 * 
		 */
		private static final long serialVersionUID = 1L;

		public void actionPerformed(ActionEvent e) {

		    TreePath path = m_tree.getSelectionPath() ;
		    DefaultMutableTreeNode node = null ;
		    FileNode fnode = null ;

		    m_clipboardAction = clipboardAction.clipboardEmpty ;

		    if (path != null) 
			node = getTreeNode(path) ;

		    if (node != null)
			fnode = getFileNode(node) ;

		    if (fnode != null) {
			File f = fnode.getFile() ;
		    }
		    m_tree.fireTreeExpanded(path) ;
		}
	    };
	m_popup.add(a01x);

	m_popup.addSeparator();

	Action a1 = new AbstractAction("Delete") {
		/**
		 * 
		 */
		private static final long serialVersionUID = 1L;

		public void actionPerformed(ActionEvent e) {
		    TreePath path = m_tree.getSelectionPath() ;
		    DefaultMutableTreeNode node = null ;
		    FileNode fnode = null ;

		    m_clipboardAction = clipboardAction.clipboardEmpty ;
		    if (path != null) 
			node = getTreeNode(path) ;
		    if (node != null)
			fnode = getFileNode(node) ;

		    if (fnode != null) {
			File f = fnode.getFile() ;

			if (!f.delete())
			    JOptionPane.showConfirmDialog
				(null, OfcExplorer.this,
				 "Unable to Delete File",
				 JOptionPane.OK_OPTION) ;
			else
			    m_model.removeNodeFromParent (node) ;
		    }
		    else
			System.out.println ("fnode is null\n") ;

		    m_tree.repaint();
		}
	    };
	m_popup.add(a1);

	Action a2 = new AbstractAction("Rename") {
		/**
		 * 
		 */
		private static final long serialVersionUID = 1L;

		// Process all files and directories under dir
		public void actionPerformed(ActionEvent e) {

		    TreePath path = m_tree.getSelectionPath() ;
		    DefaultMutableTreeNode node = null ;
		    FileNode fnode = null ;

		    m_clipboardAction = clipboardAction.clipboardEmpty ;
		    if (path != null) 
			node = getTreeNode(path) ;
		    if (node != null)
			fnode = getFileNode(node) ;

		    if (fnode != null) {
			File f = fnode.getFile() ;
			String name = f.getName() ;

			RenameFsDialog rename = new RenameFsDialog() ;
			rename.setTo(name) ;

			int retval = JOptionPane.showConfirmDialog
			    (null, rename.getComponent(),
			     "Rename " + name,
			     JOptionPane.OK_CANCEL_OPTION) ;
		    
			if (retval == JOptionPane.OK_OPTION) {
			    File f2 = 
				new File (f.getParent(), rename.getTo()) ;

			    if (!f.renameTo(f2))
				JOptionPane.showConfirmDialog
				    (null, OfcExplorer.this,
				     "Unable to Rename File",
				     JOptionPane.OK_OPTION) ;
			    else {
				node.setUserObject (new FileNode(f2)) ;
				m_model.nodeChanged(node) ;
			    }
			}
		    }
		    else
			System.out.println ("fnode is null\n") ;

		    m_tree.repaint();
		}
	    };
	m_popup.add(a2);

	Action a3 = new AbstractAction("Copy") {
		/**
		 * 
		 */
		private static final long serialVersionUID = 1L;

		public void actionPerformed(ActionEvent e) {
		    m_clipboardPath = m_tree.getSelectionPath() ;
		    m_clipboardAction = clipboardAction.clipboardCopy ;
		}
	    };
	m_popup.add(a3);

	Action a32 = new AbstractAction("Cut") {
		/**
		 * 
		 */
		private static final long serialVersionUID = 1L;

		public void actionPerformed(ActionEvent e) {
		    m_clipboardPath = m_tree.getSelectionPath() ;
		    m_clipboardAction = clipboardAction.clipboardCut ;
		}
	    };
	m_popup.add(a32);

	Action a4 = new AbstractAction("Paste") {
		/**
		 * 
		 */
		private static final long serialVersionUID = 1L;

		public void actionPerformed(ActionEvent e) {
		    if (m_clipboardAction != clipboardAction.clipboardEmpty) {
			TreePath path = m_tree.getSelectionPath() ;
			DefaultMutableTreeNode node = null ;
			FileNode fnode = null ;
			DefaultMutableTreeNode origNode = null ;
			FileNode origFnode = null ;
			File f = null ;
			File f2 = null ;

			if (m_clipboardPath != null)
			    origNode = getTreeNode(m_clipboardPath) ;
			if (origNode != null)
			    origFnode = getFileNode(origNode) ;
			if (origFnode != null)
			    f2 = origFnode.getFile() ;

			if (path != null) 
			    node = getTreeNode(path) ;
			if (node != null)
			    fnode = getFileNode(node) ;
			if (fnode != null) {
			    f = fnode.getFile() ;

			    if (f != null && !f.isDirectory()) {
				if (f instanceof File) {
				    long lastError = ((File)f).getLastError() ;
				    if (lastError != 0)
					System.out.println ("Can't Determine Directory, " +
							    "Last Error " + lastError) ;
				}

				f = f.getParentFile() ;
				node = (DefaultMutableTreeNode) 
				    node.getParent() ;
			    }
			    f = new File (f, f2.getName()) ;
			}

			// copy f2 to f
			if (f != null && f2 != null) {
			    copyFiles (f2, f) ;

			    DefaultMutableTreeNode newNode = 
				new DefaultMutableTreeNode (f.getName()) ;
			
			    newNode.setUserObject (new FileNode(f)) ;
			
			    m_model.insertNodeInto (newNode, node, 
						    node.getChildCount()) ;
			    if (m_clipboardAction == 
				clipboardAction.clipboardCut) {

				if (!f2.delete())
				    JOptionPane.showConfirmDialog
					(null, OfcExplorer.this,
					 "Unable to Delete File",
					 JOptionPane.OK_OPTION) ;
				else {
				    m_model.removeNodeFromParent (origNode) ;
				}
			    }
			}

		    }

		    m_tree.repaint();
		    m_clipboardAction = clipboardAction.clipboardEmpty ;
		}
	    };
	m_popup.add(a4);

	Action a31 = new AbstractAction("Duplicate") {
		/**
		 * 
		 */
		private static final long serialVersionUID = 1L;

		public void actionPerformed(ActionEvent e) {


		    TreePath path = m_tree.getSelectionPath() ;
		    DefaultMutableTreeNode node = null ;
		    FileNode fnode = null ;

		    m_clipboardAction = clipboardAction.clipboardEmpty ;
		    if (path != null) 
			node = getTreeNode(path) ;
		    if (node != null)
			fnode = getFileNode(node) ;
		    if (fnode != null) {
			File f = fnode.getFile() ;
			String name = "Copy of " + f.getName() ;
			File f2 = new File (f.getParent(), name) ;
		    
			copyFiles (f, f2) ;

			DefaultMutableTreeNode parent = 
			    (DefaultMutableTreeNode) node.getParent() ;
			DefaultMutableTreeNode newNode = 
			    new DefaultMutableTreeNode (name) ;

			newNode.setUserObject (new FileNode(f2)) ;

			m_model.insertNodeInto (newNode, parent, 
						parent.getChildCount()) ;
		    }

		    m_tree.repaint();
		}
	    };
	m_popup.add(a31);

	m_popup.addSeparator();

	Action a5 = new AbstractAction("Info") {
		/**
		 * 
		 */
		private static final long serialVersionUID = 1L;

		public void actionPerformed(ActionEvent e) {
		    TreePath path = m_tree.getSelectionPath() ;
		    DefaultMutableTreeNode node = getTreeNode(path) ;
		    FileNode fnode = getFileNode(node);
		    m_clipboardAction = clipboardAction.clipboardEmpty ;

		    m_tree.repaint();
		    JOptionPane.showMessageDialog
			(OfcExplorer.this, 
			 "Info option is not implemented",
			 "Info", JOptionPane.INFORMATION_MESSAGE);
		}
	    };
	m_popup.add(a5);

	m_tree.add(m_popup);

	m_tree.addMouseListener(new PopupTrigger());

	WindowListener wndCloser = new WindowAdapter()
	    {
		public void windowClosing(WindowEvent e) 
		{
		    System.exit(0);
		}
	    };
	addWindowListener(wndCloser);
    
	setVisible(true);
    }

    protected InputStream getInputStream (File a) {

	InputStream in = null ;
	boolean again = false ;

	do {
	    try {
		in = new FileInputStream (a) ;
	    }
	    catch (FileNotFoundException except) {
		System.out.println ("Failed to open input file\n") ;
	    }
	    catch (SecurityException except) {
		JOptionPane.showConfirmDialog
		    (null, OfcExplorer.this,
		     "Unable to Open Input Stream",
		     JOptionPane.OK_OPTION) ;
		again = true ;
	    }
	} while (again) ;
	return in ;
    }
		    
    protected OutputStream getOutputStream (File b) {

	OutputStream out = null ;
	boolean again ;

	do {
	    again = false ;
	    try {
		out = new FileOutputStream (b) ;
	    }
	    catch (FileNotFoundException except) {
		System.out.println ("Failed to open output file\n") ;
	    }
	    catch (SecurityException except) {
		JOptionPane.showConfirmDialog
		    (null, OfcExplorer.this,
		     "Unable to Open Output Stream",
		     JOptionPane.OK_OPTION) ;
		again = true ;
	    }
	} while (again) ;
	return out ;
    }

    protected boolean copyFiles (File a, File b) {
	
	boolean ret ;
	InputStream in = null ;
	OutputStream out = null ;

	ret = false ;
	
	in = getInputStream (a) ;
	if (in != null) {
	    out = getOutputStream (b) ;
	    if (out != null) {
		int len;
		byte[] buf = new byte[1024] ;

		try {
		    do {
			len = in.read(buf) ;
			if (len > 0) {
			    out.write(buf, 0, len);
			}
		    } while (len > 0) ;
		    ret = true ;
		}
		catch (IOException except) {
		    System.out.println("IO Error on copy\n") ;
		}

		try {
		    out.close() ;
		}
		catch (IOException except) {
		}
	    }
	    try {
		in.close() ;
	    }
	    catch (IOException except) {
	    }
	}
	return ret ;
    }

    DefaultMutableTreeNode getTreeNode(TreePath path)
    {
	return (DefaultMutableTreeNode)(path.getLastPathComponent());
    }

    FileNode getFileNode(DefaultMutableTreeNode node)
    {
	if (node == null)
	    return null;
	Object obj = node.getUserObject();
	if (obj instanceof IconData)
	    obj = ((IconData)obj).getObject();
	if (obj instanceof FileNode)
	    return (FileNode)obj;
	else
	    return null;
    }


    public class RenameFsDialog {
	private JPanel mainPanel = new JPanel() ;
	private JTextField to = new JTextField(20) ;

	public RenameFsDialog () {

	    mainPanel.setLayout (new GridLayout (2, 2, 5, 5)) ;
	    mainPanel.add(new JLabel("To File:")) ;
	    mainPanel.add(to) ;
	}

	public String getTo() {
	    return to.getText() ;
	}

	public void setTo(String text) {
	    to.setText(text) ;
	}

	public JComponent getComponent() {
	    return mainPanel;
	}
    }

    public class Editor extends JFrame {
	/**
		 * 
		 */
		private static final long serialVersionUID = 1L;

	public Editor (File f) {
	    setTitle(f.getPath()) ;
	    setSize(600, 400) ;
	    InputStream in = getInputStream (f) ;
	    if (in != null) {
		JEditorPane ed = new JEditorPane() ;

		try {
		    ed.setContentType("text/plain") ;
		    ed.read(in, null) ;
		}
		catch (IOException except) {
		    System.out.println ("Couldn't read\n") ;
		    System.out.println (except.getMessage() + "\n") ;
		}
		JScrollPane scrollPane = new JScrollPane(ed) ;
		getContentPane().add(scrollPane, BorderLayout.CENTER);
		setVisible(true);
		try {
		    in.close() ;
		}
		catch (IOException except) {
		}

	    }
	}
    }

    public class Jpeg extends Panel {

	Image  image;
	public Jpeg(File f) {
	    try {
		InputStream in = getInputStream (f) ;
		BufferedImage ximage = ImageIO.read(in);
		Dimension d = Toolkit.getDefaultToolkit().getScreenSize() ;
		image = ximage.getScaledInstance (d.width, d.height, Image.SCALE_FAST) ;
	    } catch (IOException ie) {
		System.out.println("Error:"+ie.getMessage());
	    }
	}

	public void paint(Graphics g) {
	    g.drawImage (image, 0, 0, null) ;
	}
    }

    class PopupTrigger extends MouseAdapter
    {
	public void mousePressed(MouseEvent e) {
	    if (e.isPopupTrigger()) {
		int x = e.getX();
		int y = e.getY();
		TreePath path = m_tree.getPathForLocation(x, y);
		if (path != null) {
		    if (m_tree.isExpanded(path))
			m_action.putValue(Action.NAME, "Collapse");
		    else
			m_action.putValue(Action.NAME, "Expand");
		    m_popup.show(m_tree, x, y);
		    m_clickedPath = path;
		}
	    }
	}

	public void mouseReleased(MouseEvent e)
	{
	    if (e.isPopupTrigger())
		{
		    int x = e.getX();
		    int y = e.getY();
		    TreePath path = m_tree.getPathForLocation(x, y);
		    if (path != null)
			{
			    if (m_tree.isExpanded(path))
				m_action.putValue(Action.NAME, "Collapse");
			    else
				m_action.putValue(Action.NAME, "Expand");
			    m_popup.show(m_tree, x, y);
			    m_clickedPath = path;
			}
	
		}
	}
    }

    // Make sure expansion is threaded and updating the tree model
    // only occurs within the event dispatching thread.
    class DirExpansionListener implements TreeExpansionListener
    {
        public void treeExpanded(TreeExpansionEvent event)
        {
            final DefaultMutableTreeNode node = getTreeNode(
							    event.getPath());
            final FileNode fnode = getFileNode(node);

            Thread runner = new Thread() 
		{
		    public void run() 
		    {
			if (fnode != null && fnode.expand(node, m_tree)) 
			    {
				Runnable runnable = new Runnable() 
				    {
					public void run() 
					{
					    m_model.reload(node);
					}
				    };
				SwingUtilities.invokeLater(runnable);
			    }
		    }
		};
            runner.start();
        }

        public void treeCollapsed(TreeExpansionEvent event) {}
    }

    class DirSelectionListener 
	implements TreeSelectionListener 
    {
	public void valueChanged(TreeSelectionEvent event)
	{
	    DefaultMutableTreeNode node = getTreeNode(
						      event.getPath());
	    FileNode fnode = getFileNode(node);
	    if (fnode != null) {
		m_display.setText(fnode.getFile().
				  getAbsolutePath());
	    }
	    else
		m_display.setText("");
	}
    }

    class NodeModelListener 
	implements TreeModelListener {

	public void treeNodesChanged(TreeModelEvent event) {

	    TreePath path = m_tree.getSelectionPath() ;
	    DefaultMutableTreeNode node = null ;
	    FileNode fnode = null ;
	    File f = null ;

	    if (path != null) 
		node = getTreeNode(path) ;
	    if (node != null)
		fnode = getFileNode(node) ;

	    if (fnode != null) 
		f = fnode.getFile() ;

	    if (f == null) {
		System.out.println ("file is null\n") ;
	    }
	}
	public void treeNodesInserted(TreeModelEvent event) {
	}
	public void treeNodesRemoved(TreeModelEvent event) {
	}
	public void treeStructureChanged(TreeModelEvent event) {
	}
    }

    public static void main(String argv[]) 
    {
	new OfcExplorer(argv[0]);
    }
}

class IconCellRenderer 
    extends    JLabel 
    implements TreeCellRenderer
{
    /**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	protected Color m_textSelectionColor;
    protected Color m_textNonSelectionColor;
    protected Color m_bkSelectionColor;
    protected Color m_bkNonSelectionColor;
    protected Color m_borderSelectionColor;

    protected boolean m_selected;

    public IconCellRenderer()
    {
	super();
	m_textSelectionColor = UIManager.getColor(
						  "Tree.selectionForeground");
	m_textNonSelectionColor = UIManager.getColor(
						     "Tree.textForeground");
	m_bkSelectionColor = UIManager.getColor(
						"Tree.selectionBackground");
	m_bkNonSelectionColor = UIManager.getColor(
						   "Tree.textBackground");
	m_borderSelectionColor = UIManager.getColor(
						    "Tree.selectionBorderColor");
	setOpaque(false);
    }

    public Component getTreeCellRendererComponent(JTree tree, 
						  Object value, boolean sel, boolean expanded, boolean leaf, 
						  int row, boolean hasFocus) 
    
    {
	DefaultMutableTreeNode node = 
	    (DefaultMutableTreeNode)value;
	Object obj = node.getUserObject();
	setText(obj.toString());

	if (obj instanceof Boolean)
	    setText("Retrieving data...");

	if (obj instanceof IconData)
	    {
		IconData idata = (IconData)obj;
		if (expanded)
		    setIcon(idata.getExpandedIcon());
		else
		    setIcon(idata.getIcon());
	    }
	else
	    setIcon(null);

	setFont(tree.getFont());
	setForeground(sel ? m_textSelectionColor : 
		      m_textNonSelectionColor);
	setBackground(sel ? m_bkSelectionColor : 
		      m_bkNonSelectionColor);
	m_selected = sel;
	return this;
    }
    
    public void paintComponent(Graphics g) 
    {
	Color bColor = getBackground();
	Icon icon = getIcon();

	g.setColor(bColor);
	int offset = 0;
	if(icon != null && getText() != null) 
	    offset = (icon.getIconWidth() + getIconTextGap());
	g.fillRect(offset, 0, getWidth() - 1 - offset,
		   getHeight() - 1);
    
	if (m_selected) 
	    {
		g.setColor(m_borderSelectionColor);
		g.drawRect(offset, 0, getWidth()-1-offset, getHeight()-1);
	    }

	super.paintComponent(g);
    }
}

class IconData
{
    protected Icon   m_icon;
    protected Icon   m_expandedIcon;
    protected Object m_data;

    public IconData(Icon icon, Object data)
    {
	m_icon = icon;
	m_expandedIcon = null;
	m_data = data;
    }

    public IconData(Icon icon, Icon expandedIcon, Object data)
    {
	m_icon = icon;
	m_expandedIcon = expandedIcon;
	m_data = data;
    }

    public Icon getIcon() 
    { 
	return m_icon;
    }

    public Icon getExpandedIcon() 
    { 
	return m_expandedIcon!=null ? m_expandedIcon : m_icon;
    }

    public Object getObject() 
    { 
	return m_data;
    }

    public String toString() 
    { 
	return m_data.toString();
    }
}

class FileNode
{
    protected File m_file;

    public FileNode(File file)
    {
	m_file = file;
    }

    public File getFile() 
    { 
	return m_file;
    }

    public String toString() 
    { 
	return m_file.getName().length() > 0 ? m_file.getName() : 
	    m_file.getPath();
    }

    public boolean expand(DefaultMutableTreeNode parent,
			  JTree  m_tree)
    {
	DefaultMutableTreeNode flag ;
	try {
	    flag = (DefaultMutableTreeNode)parent.getFirstChild();
	    if (flag==null)    // No flag
		return false;

	    parent.removeAllChildren();  // Remove Flag
	} catch (Exception ex) {
	}

	File[] files = listFiles();
	if (files == null)
	    return true;

	Vector<FileNode> v = new Vector<FileNode>();

	for (int k=0; k<files.length; k++)
	    {
		File f = files[k];

		if (!f.isHidden()) {
		    FileNode newNode = new FileNode(f);
		    boolean isAdded = false;
		    for (int i=0; i<v.size(); i++)
			{
			    FileNode nd = (FileNode)v.elementAt(i);
			    if (newNode.compareTo(nd) < 0)
				{
				    v.insertElementAt(newNode, i);
				    isAdded = true;
				    break;
				}
			}
		    if (!isAdded)
			v.addElement(newNode);
		}
	    }

	if (v.size() > 0)
	    {
		for (int i=0; i<v.size(); i++) {
		    FileNode nd = (FileNode)v.elementAt(i);
		    IconData idata = 
			new IconData(OfcExplorer.ICON_FOLDER, 
				     OfcExplorer.ICON_EXPANDEDFOLDER, nd);
		    DefaultMutableTreeNode node = new 
			DefaultMutableTreeNode(idata);

		    parent.add(node);
        
		    if (nd.isDir()) {
			node.add(new DefaultMutableTreeNode(new Boolean(true) ));
		    }
		}
	    }
	else
	    {
		// None of this seems to work.  I'd love to leave the item 
		// so that it was expandable (with the +) but I can't seem
		// to get that to work.  
		//DefaultMutableTreeNode node =  
		//new DefaultMutableTreeNode(new Boolean(true) );
		// parent.add(node);
		// m_tree.collapsePath(m_tree.getSelectionPath()) ;
		//m_tree.fireTreeCollapsed(m_tree.getSelectionPath()) ;
	    }
	return true;
    }

    public boolean isDir() {
	boolean ret = m_file.isDirectory() ;

	return (ret) ;
    }

    public boolean hasSubDirs()
    {
	File[] files = listFiles();
	if (files == null)
	    return false;
	for (int k=0; k<files.length; k++)
	    {
		if (!files[k].isHidden() && files[k].isDirectory())
		    return true;
	    }
	return false;
    }
  
    public int compareTo(FileNode toCompare)
    { 
	return  m_file.getName().compareToIgnoreCase(
						     toCompare.m_file.getName() ); 
    }

    protected File[] listFiles()
    {
	if (!m_file.isDirectory()) {
	    return null;
	}

	try {
	    File[] files ;

	    files = m_file.listFiles() ;

	    if (files.length == 0) {
		if (m_file instanceof File) {
		    long errorcode =
			((File) m_file).getLastError() ;
		    if (errorcode == 58) {
			JOptionPane.showMessageDialog
			    (null, 
			     "No Local Master Browser Found.  " +
			     "Cannot Browse Network",
			     "Warning", JOptionPane.WARNING_MESSAGE);
		    }
		    else if (errorcode != 0) {
			JOptionPane.showMessageDialog
			    (null, 
			     "Error reading directory " +
			     m_file.getAbsolutePath() +
			     " Error " + errorcode,
			     "Warning", JOptionPane.WARNING_MESSAGE);
			return files;
		    }
		}
	    }
	    return files ;
	}
	catch (Exception ex) {
	    JOptionPane.showMessageDialog
		(null, 
		 "Error reading directory "+m_file.getAbsolutePath(),
		 "Warning", JOptionPane.WARNING_MESSAGE);
	    return null;
	}
    }
}
