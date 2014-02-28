/*
 * Лицензионное соглашение на использование набора средств разработки
 * «SDK Яндекс.Диска» доступно по адресу: http://legal.yandex.ru/sdk_agreement
 *
 */

package com.rraptor.pult.yad;

import java.io.File;
import java.util.List;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.support.v4.app.ListFragment;
import android.support.v4.app.LoaderManager;
import android.support.v4.content.Loader;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;

import com.rraptor.pult.R;
import com.yandex.disk.client.Credentials;
import com.yandex.disk.client.ListItem;

public class ListExampleFragment extends ListFragment implements
		LoaderManager.LoaderCallbacks<List<ListItem>> {

	public static class ListExampleAdapter extends ArrayAdapter<ListItem> {
		private final LayoutInflater inflater;

		public ListExampleAdapter(Context context) {
			super(context, android.R.layout.simple_list_item_2);
			inflater = (LayoutInflater) context
					.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
		}

		@Override
		public View getView(int position, View convertView, ViewGroup parent) {
			View view;

			if (convertView == null) {
				view = inflater.inflate(android.R.layout.simple_list_item_2,
						parent, false);
			} else {
				view = convertView;
			}

			ListItem item = getItem(position);
			((TextView) view.findViewById(android.R.id.text1)).setText(item
					.getDisplayName());
			((TextView) view.findViewById(android.R.id.text2)).setText(item
					.isCollection() ? "" : "" + item.getContentLength());

			return view;
		}

		public void setData(List<ListItem> data) {
			clear();
			if (data != null) {
				// addAll(data);
				for (ListItem item : data) {
					add(item);
				}
			}
		}
	}

	private static final String TAG = "ListExampleFragment";

	private static final String CURRENT_DIR_KEY = "example.current.dir";

	private static final String ROOT = "/";

	private Credentials credentials;

	private String currentDir;

	private ListExampleAdapter adapter;

	protected void changeDir(String dir) {
		Bundle args = new Bundle();
		args.putString(CURRENT_DIR_KEY, dir);

		ListExampleFragment fragment = new ListExampleFragment();
		fragment.setArguments(args);

		getFragmentManager()
				.beginTransaction()
				.replace(android.R.id.content, fragment,
						YaDiskActivity.FRAGMENT_TAG).addToBackStack(null)
				.commit();
	}

	private void downloadFile(ListItem item) {
		DownloadFileFragment
				.newInstance(credentials, item)
				.setDownloadListener(
						new DownloadFileFragment.DownloadListener() {

							@Override
							public void downloadComplete(File file) {
								Intent data = new Intent();
								data.putExtra(
										YaDiskActivity.KEY_DOWNLOADED_FILE_PATH,
										file.getPath());
								getActivity().setResult(Activity.RESULT_OK,
										data);
								getActivity().finish();
							}
						}).show(getFragmentManager(), "download");
	}

	@Override
	public void onActivityCreated(Bundle savedInstanceState) {
		super.onActivityCreated(savedInstanceState);

		setDefaultEmptyText();

		setHasOptionsMenu(true);

		registerForContextMenu(getListView());

		SharedPreferences preferences = PreferenceManager
				.getDefaultSharedPreferences(getActivity());
		String username = preferences.getString(YaDiskActivity.USERNAME, null);
		String token = preferences.getString(YaDiskActivity.TOKEN, null);

		credentials = new Credentials(username, token);

		Bundle args = getArguments();
		if (args != null) {
			currentDir = args.getString(CURRENT_DIR_KEY);
		}
		if (currentDir == null) {
			currentDir = ROOT;
		}
		// getActivity().getActionBar().setDisplayHomeAsUpEnabled(!ROOT.equals(currentDir));

		adapter = new ListExampleAdapter(getActivity());
		setListAdapter(adapter);
		setListShown(false);
		getLoaderManager().initLoader(0, null, this);
	}

	@Override
	public Loader<List<ListItem>> onCreateLoader(int i, Bundle bundle) {
		return new ListExampleLoader(getActivity(), credentials, currentDir);
	}

	@Override
	public void onCreateOptionsMenu(Menu menu, MenuInflater inflater) {
		super.onCreateOptionsMenu(menu, inflater);

		// inflater.inflate(R.menu.example_action_bar, menu);
	}

	@Override
	public void onListItemClick(ListView listView, View view, int position,
			long id) {
		ListItem item = (ListItem) getListAdapter().getItem(position);
		Log.d(TAG, "onListItemClick(): " + item);
		if (item.isCollection()) {
			changeDir(item.getFullPath());
		} else {
			downloadFile(item);
		}
	}

	@Override
	public void onLoaderReset(Loader<List<ListItem>> loader) {
		adapter.setData(null);
	}

	@Override
	public void onLoadFinished(final Loader<List<ListItem>> loader,
			List<ListItem> data) {
		if (isResumed()) {
			setListShown(true);
		} else {
			setListShownNoAnimation(true);
		}
		if (data.isEmpty()) {
			Exception ex = ((ListExampleLoader) loader).getException();
			if (ex != null) {
				setEmptyText(((ListExampleLoader) loader).getException()
						.getMessage());
			} else {
				setDefaultEmptyText();
			}
		} else {
			adapter.setData(data);
		}
	}

	public void restartLoader() {
		getLoaderManager().restartLoader(0, null, this);
	}

	private void setDefaultEmptyText() {
		setEmptyText(getString(R.string.example_no_files));
	}
}
