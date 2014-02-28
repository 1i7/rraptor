/*
 * Лицензионное соглашение на использование набора средств разработки
 * «SDK Яндекс.Диска» доступно по адресу: http://legal.yandex.ru/sdk_agreement
 *
 */

package com.rraptor.pult.yad;

import java.io.File;
import java.io.IOException;

import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.support.v4.app.FragmentManager;
import android.util.Log;

import com.rraptor.pult.R;
import com.yandex.disk.client.Credentials;
import com.yandex.disk.client.ListItem;
import com.yandex.disk.client.ProgressListener;
import com.yandex.disk.client.TransportClient;
import com.yandex.disk.client.exceptions.WebdavException;

public class DownloadFileFragment extends IODialogFragment {

	public static class DownloadFileRetainedFragment extends
			IODialogRetainedFragment implements ProgressListener {

		private boolean cancelled;
		private File result;

		public void cancelDownload() {
			cancelled = true;
		}

		public void downloadComplete() {
			handler.post(new Runnable() {
				@Override
				public void run() {
					DownloadFileFragment targetFragment = (DownloadFileFragment) getTargetFragment();
					if (targetFragment != null) {
						targetFragment.onDownloadComplete(result);
					}
				}
			});
		}

		@Override
		public boolean hasCancelled() {
			return cancelled;
		}

		public void loadFile(final Context context,
				final Credentials credentials, final ListItem item) {
			result = new File(context.getFilesDir(), new File(
					item.getFullPath()).getName());

			new Thread(new Runnable() {
				@Override
				public void run() {
					TransportClient client = null;
					try {
						client = TransportClient.getInstance(context,
								credentials);
						client.downloadFile(item.getFullPath(), result,
								DownloadFileRetainedFragment.this);
						downloadComplete();
					} catch (IOException ex) {
						Log.d(TAG, "loadFile", ex);
						sendException(ex);
					} catch (WebdavException ex) {
						Log.d(TAG, "loadFile", ex);
						sendException(ex);
					} finally {
						if (client != null) {
							client.shutdown();
						}
					}
				}
			}).start();
		}

		@Override
		public void updateProgress(final long loaded, final long total) {
			handler.post(new Runnable() {
				@Override
				public void run() {
					DownloadFileFragment targetFragment = (DownloadFileFragment) getTargetFragment();
					if (targetFragment != null) {
						targetFragment.setDownloadProgress(loaded, total);
					}
				}
			});
		}
	}

	public static interface DownloadListener {
		void downloadComplete(final File file);
	}

	private DownloadListener downloadListener;

	private static final String TAG = "LoadFileFragment";

	private static final String WORK_FRAGMENT_TAG = "LoadFileFragment.Background";

	private static final String FILE_ITEM = "example.file.item";

	private static final int PROGRESS_DIV = 1024 * 1024;

	public static DownloadFileFragment newInstance(Credentials credentials,
			ListItem item) {
		DownloadFileFragment fragment = new DownloadFileFragment();

		Bundle args = new Bundle();
		args.putParcelable(CREDENTIALS, credentials);
		args.putParcelable(FILE_ITEM, item);
		fragment.setArguments(args);

		return fragment;
	}

	private Credentials credentials;

	private ListItem item;

	private DownloadFileRetainedFragment workFragment;

	private void makeWorldReadableAndMakePublic(File file) {
		file.setReadable(true, false);
		downloadListener.downloadComplete(file);

		// open file in external program
		// Intent intent = new Intent();
		// intent.setAction(Intent.ACTION_VIEW);
		// intent.setDataAndType(Uri.fromFile(file), item.getContentType());
		// startActivity(Intent.createChooser(intent,
		// getText(R.string.example_loading_file_chooser_title)));
	}

	@Override
	public void onActivityCreated(Bundle savedInstanceState) {
		super.onActivityCreated(savedInstanceState);

		FragmentManager fragmentManager = getFragmentManager();
		workFragment = (DownloadFileRetainedFragment) fragmentManager
				.findFragmentByTag(WORK_FRAGMENT_TAG);
		if (workFragment == null || workFragment.getTargetFragment() == null) {
			workFragment = new DownloadFileRetainedFragment();
			fragmentManager.beginTransaction()
					.add(workFragment, WORK_FRAGMENT_TAG).commit();
			workFragment.loadFile(getActivity(), credentials, item);
		}
		workFragment.setTargetFragment(this, 0);
	}

	private void onCancel() {
		workFragment.cancelDownload();
	}

	@Override
	public void onCancel(DialogInterface dialog) {
		super.onCancel(dialog);
		onCancel();
	}

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		credentials = getArguments().getParcelable(CREDENTIALS);
		item = getArguments().getParcelable(FILE_ITEM);
	}

	@Override
	public Dialog onCreateDialog(Bundle savedInstanceState) {
		dialog = new ProgressDialog(getActivity());
		dialog.setTitle(R.string.example_loading_file_title);
		dialog.setMessage(item.getDisplayName());
		dialog.setProgressStyle(ProgressDialog.STYLE_HORIZONTAL);
		dialog.setIndeterminate(true);
		dialog.setButton(ProgressDialog.BUTTON_NEUTRAL,
				getString(R.string.example_loading_file_cancel_button),
				new DialogInterface.OnClickListener() {
					@Override
					public void onClick(DialogInterface dialog, int which) {
						dialog.dismiss();
						onCancel();
					}
				});
		dialog.setOnCancelListener(this);
		dialog.show();
		return dialog;
	}

	@Override
	public void onDetach() {
		super.onDetach();

		if (workFragment != null) {
			workFragment.setTargetFragment(null, 0);
		}
	}

	public void onDownloadComplete(File file) {
		dialog.dismiss();
		makeWorldReadableAndMakePublic(file);
	}

	public DownloadFileFragment setDownloadListener(DownloadListener listener) {
		this.downloadListener = listener;
		return this;
	}

	public void setDownloadProgress(long loaded, long total) {
		if (dialog != null) {
			if (dialog.isIndeterminate()) {
				dialog.setIndeterminate(false);
			}
			if (total > Integer.MAX_VALUE) {
				dialog.setProgress((int) (loaded / PROGRESS_DIV));
				dialog.setMax((int) (total / PROGRESS_DIV));
			} else {
				dialog.setProgress((int) loaded);
				dialog.setMax((int) total);
			}
		}
	}
}
