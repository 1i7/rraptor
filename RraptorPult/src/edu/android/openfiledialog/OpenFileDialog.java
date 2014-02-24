package edu.android.openfiledialog;

import java.io.File;
import java.io.FilenameFilter;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.graphics.Paint;
import android.graphics.Point;
import android.graphics.Rect;
import android.graphics.drawable.Drawable;
import android.os.Environment;
import android.util.DisplayMetrics;
import android.util.TypedValue;
import android.view.Display;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

/**
 * Created with IntelliJ IDEA. User: Scogun Date: 27.11.13 Time: 10:47
 * 
 * https://github.com/Scogun/Android-OpenFileDialog
 * http://habrahabr.ru/post/203884/
 */
public class OpenFileDialog extends AlertDialog.Builder {

	private class FileAdapter extends ArrayAdapter<File> {

		public FileAdapter(Context context, List<File> files) {
			super(context, android.R.layout.simple_list_item_1, files);
		}

		@Override
		public View getView(int position, View convertView, ViewGroup parent) {
			TextView view = (TextView) super.getView(position, convertView,
					parent);
			File file = getItem(position);
			if (view != null) {
				view.setText(file.getName());
				if (file.isDirectory()) {
					setDrawable(view, folderIcon);
				} else {
					setDrawable(view, fileIcon);
					if (selectedIndex == position)
						view.setBackgroundColor(getContext().getResources()
								.getColor(android.R.color.holo_blue_dark));
					else
						view.setBackgroundColor(getContext().getResources()
								.getColor(android.R.color.transparent));
				}
			}
			return view;
		}

		private void setDrawable(TextView view, Drawable drawable) {
			if (view != null) {
				if (drawable != null) {
					drawable.setBounds(0, 0, 60, 60);
					view.setCompoundDrawables(drawable, null, null, null);
				} else {
					view.setCompoundDrawables(null, null, null, null);
				}
			}
		}
	}

	public interface OpenDialogListener {
		public void OnSelectedFile(String fileName);
	}

	private static Display getDefaultDisplay(Context context) {
		return ((WindowManager) context
				.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
	}

	private static int getLinearLayoutMinHeight(Context context) {
		return getScreenSize(context).y;
	}

	private static Point getScreenSize(Context context) {
		DisplayMetrics dm = new DisplayMetrics();
		getDefaultDisplay(context).getMetrics(dm);
		return new Point(dm.widthPixels, dm.heightPixels);
	}

	private String currentPath = Environment.getExternalStorageDirectory()
			.getPath();
	private final List<File> files = new ArrayList<File>();
	private final TextView title;
	private final ListView listView;
	private FilenameFilter filenameFilter;

	private int selectedIndex = -1;

	private OpenDialogListener listener;

	private Drawable folderIcon;

	private Drawable fileIcon;

	private String accessDeniedMessage;

	private final Context context;

	public OpenFileDialog(Context context) {
		this(context, Environment.getExternalStorageDirectory().getPath());
	}

	public OpenFileDialog(Context context, String currentPath) {
		super(context);
		this.context = context;
		this.currentPath = currentPath;
		title = createTitle(context);
		changeTitle();
		LinearLayout linearLayout = createMainLayout(context);
		linearLayout.addView(createBackItem(context));
		listView = createListView(context);
		linearLayout.addView(listView);
		setCustomTitle(title)
				.setView(linearLayout)
				.setPositiveButton(android.R.string.ok,
						new DialogInterface.OnClickListener() {
							@Override
							public void onClick(DialogInterface dialog,
									int which) {
								if (selectedIndex > -1 && listener != null) {
									listener.OnSelectedFile(listView
											.getItemAtPosition(selectedIndex)
											.toString());
								}
							}
						}).setNegativeButton(android.R.string.cancel, null);
	}

	private void changeTitle() {
		String titleText = currentPath;
		int screenWidth = getScreenSize(getContext()).x;
		int maxWidth = (int) (screenWidth * 0.99);
		if (getTextWidth(titleText, title.getPaint()) > maxWidth) {
			while (getTextWidth("..." + titleText, title.getPaint()) > maxWidth) {
				int start = titleText.indexOf("/", 2);
				if (start > 0)
					titleText = titleText.substring(start);
				else
					titleText = titleText.substring(2);
			}
			title.setText("..." + titleText);
		} else {
			title.setText(titleText);
		}
	}

	private TextView createBackItem(Context context) {
		TextView textView = createTextView(context,
				android.R.style.TextAppearance_DeviceDefault_Small);
		Drawable drawable = getContext().getResources().getDrawable(
				android.R.drawable.ic_menu_directions);
		drawable.setBounds(0, 0, 60, 60);
		textView.setCompoundDrawables(drawable, null, null, null);
		textView.setLayoutParams(new ViewGroup.LayoutParams(
				ViewGroup.LayoutParams.MATCH_PARENT,
				ViewGroup.LayoutParams.WRAP_CONTENT));
		textView.setOnClickListener(new View.OnClickListener() {

			@Override
			public void onClick(View view) {
				File file = new File(currentPath);
				File parentDirectory = file.getParentFile();
				if (parentDirectory != null) {
					currentPath = parentDirectory.getPath();
					RebuildFiles(((FileAdapter) listView.getAdapter()));
				}
			}
		});
		return textView;
	}

	private ListView createListView(Context context) {
		ListView listView = new ListView(context);
		listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {

			@Override
			public void onItemClick(AdapterView<?> adapterView, View view,
					int index, long l) {
				final ArrayAdapter<File> adapter = (FileAdapter) adapterView
						.getAdapter();
				File file = adapter.getItem(index);
				if (file.isDirectory()) {
					currentPath = file.getPath();
					RebuildFiles(adapter);
				} else {
					if (index != selectedIndex)
						selectedIndex = index;
					else
						selectedIndex = -1;
					adapter.notifyDataSetChanged();
				}
			}
		});
		return listView;
	}

	private LinearLayout createMainLayout(Context context) {
		LinearLayout linearLayout = new LinearLayout(context);
		linearLayout.setOrientation(LinearLayout.VERTICAL);
		linearLayout.setMinimumHeight(getLinearLayoutMinHeight(context));
		return linearLayout;
	}

	private TextView createTextView(Context context, int style) {
		TextView textView = new TextView(context);
		textView.setTextAppearance(context, style);
		int itemHeight = getItemHeight(context);
		textView.setLayoutParams(new ViewGroup.LayoutParams(
				ViewGroup.LayoutParams.MATCH_PARENT, itemHeight));
		textView.setMinHeight(itemHeight);
		textView.setGravity(Gravity.CENTER_VERTICAL);
		textView.setPadding(15, 0, 0, 0);
		return textView;
	}

	private TextView createTitle(Context context) {
		TextView textView = createTextView(context,
				android.R.style.TextAppearance_DeviceDefault_DialogWindowTitle);
		return textView;
	}

	/**
	 * Implement for API level 10 compatibility, delete it for higher versions
	 * of sdk
	 * 
	 * @return
	 */
	@Override
	public Context getContext() {
		return context;
	}

	private List<File> getFiles(String directoryPath) {
		File directory = new File(directoryPath);
		List<File> fileList = Arrays
				.asList(directory.listFiles(filenameFilter));
		Collections.sort(fileList, new Comparator<File>() {
			@Override
			public int compare(File file, File file2) {
				if (file.isDirectory() && file2.isFile())
					return -1;
				else if (file.isFile() && file2.isDirectory())
					return 1;
				else
					return file.getPath().compareTo(file2.getPath());
			}
		});
		return fileList;
	}

	private int getItemHeight(Context context) {
		TypedValue value = new TypedValue();
		DisplayMetrics metrics = new DisplayMetrics();
		context.getTheme().resolveAttribute(
				android.R.attr.listPreferredItemHeightSmall, value, true);
		getDefaultDisplay(context).getMetrics(metrics);
		return (int) TypedValue.complexToDimension(value.data, metrics);
	}

	public int getTextWidth(String text, Paint paint) {
		Rect bounds = new Rect();
		paint.getTextBounds(text, 0, text.length(), bounds);
		return bounds.left + bounds.width() + 80;
	}

	private void RebuildFiles(ArrayAdapter<File> adapter) {
		try {
			List<File> fileList = getFiles(currentPath);
			files.clear();
			selectedIndex = -1;
			files.addAll(fileList);
			adapter.notifyDataSetChanged();
			changeTitle();
		} catch (NullPointerException e) {
			String message = getContext().getResources().getString(
					android.R.string.unknownName);
			if (!accessDeniedMessage.equals(""))
				message = accessDeniedMessage;
			Toast.makeText(getContext(), message, Toast.LENGTH_SHORT).show();
		}
	}

	public OpenFileDialog setAccessDeniedMessage(String message) {
		this.accessDeniedMessage = message;
		return this;
	}

	public OpenFileDialog setFileIcon(Drawable drawable) {
		this.fileIcon = drawable;
		return this;
	}

	public OpenFileDialog setFilter(final String filter) {
		filenameFilter = new FilenameFilter() {

			@Override
			public boolean accept(File file, String fileName) {
				File tempFile = new File(String.format("%s/%s", file.getPath(),
						fileName));
				if (tempFile.isFile())
					return tempFile.getName().matches(filter);
				return true;
			}
		};
		return this;
	}

	public OpenFileDialog setFolderIcon(Drawable drawable) {
		this.folderIcon = drawable;
		return this;
	}

	public OpenFileDialog setOpenDialogListener(OpenDialogListener listener) {
		this.listener = listener;
		return this;
	}

	@Override
	public AlertDialog show() {
		files.addAll(getFiles(currentPath));
		listView.setAdapter(new FileAdapter(getContext(), files));
		return super.show();
	}
}
