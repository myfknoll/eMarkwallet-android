package com.breadwallet.presenter.fragments;

import android.app.AlertDialog;
import android.app.Fragment;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.SystemClock;
import android.provider.Settings;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.EditorInfo;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.breadwallet.R;
import com.breadwallet.BreadWalletApp;
import com.breadwallet.presenter.activities.IntroActivity;
import com.breadwallet.tools.manager.SharedPreferencesManager;
import com.breadwallet.tools.util.Utils;
import com.breadwallet.tools.util.WordsReader;
import com.breadwallet.tools.security.PostAuthenticationProcessor;
import com.breadwallet.wallet.BRWalletManager;

import java.util.List;


/**
 * BreadWallet
 * <p>
 * Created by Mihail Gutan <mihail@breadwallet.com> on 9/15/15.
 * Copyright (c) 2016 breadwallet LLC
 * <p>
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * <p>
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * <p>
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

public class IntroRecoverWalletFragment extends Fragment {
    public static final String TAG = IntroRecoverWalletFragment.class.getName();
    private Button recoverButton;
    private EditText editText;
    private AlertDialog alertDialog;

    @Override
    public View onCreateView(LayoutInflater inflater,
                             ViewGroup container, Bundle savedInstanceState) {
        // The last two arguments ensure LayoutParams are inflated
        // properly.
        View rootView = inflater.inflate(
                R.layout.intro_fragment_recover_wallet, container, false);

        recoverButton = (Button) rootView.findViewById(R.id.recover_button);
        editText = (EditText) rootView.findViewById(R.id.recover_wallet_edit_text);
        editText.setText("");
        editText.setOnEditorActionListener(new TextView.OnEditorActionListener() {
            public boolean onEditorAction(TextView v, int actionId, KeyEvent event) {
                if ((event != null && (event.getKeyCode() == KeyEvent.KEYCODE_ENTER)) ||
                        (actionId == EditorInfo.IME_ACTION_DONE)) {
                    recoverButton.performClick();
                }
                return false;
            }
        });
        alertDialog = new AlertDialog.Builder(getActivity()).create();
        recoverButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (alertDialog.isShowing()) {
                    alertDialog.dismiss();
                }

                String phraseToCheck = editText.getText().toString().toLowerCase();
                String cleanPhrase = WordsReader.cleanPhrase(getActivity(), phraseToCheck);

                if (BRWalletManager.getInstance(getActivity()).validatePhrase(getActivity(), cleanPhrase)) {
                    showKeyBoard(false);
                    BRWalletManager m = BRWalletManager.getInstance(getActivity());
                    m.wipeWalletButKeystore(getActivity());
                    m.wipeKeyStore();
                    PostAuthenticationProcessor.getInstance().setPhraseForKeyStore(cleanPhrase);
                    PostAuthenticationProcessor.getInstance().onRecoverWalletAuth((IntroActivity) getActivity());
                    SharedPreferencesManager.putAllowSpend(getActivity(),false);

                } else {
                    String message = getResources().getString(R.string.bad_recovery_phrase);
                    String[] words = cleanPhrase.split(" ");
                    if (words.length != 12) {
                        message = String.format(getActivity().getString(R.string.recovery_phrase_must_have_12_words),12);
                    } else {
                        List<String> allWords = WordsReader.getAllWordLists(getActivity());

                        for (String word : words) {
                            if (!allWords.contains(word)) {
                                message = String.format(getActivity().getString(R.string.not_a_recovery_phrase_word), word);
                            }
                        }
                    }

                    //don't use
                    alertDialog.setMessage(message);
                    alertDialog.setButton(AlertDialog.BUTTON_NEUTRAL, getResources().getString(R.string.ok),
                            new DialogInterface.OnClickListener() {
                                public void onClick(DialogInterface dialog, int which) {
                                    dialog.dismiss();
                                }
                            });
                    alertDialog.show();
                }
            }
        });

        return rootView;
    }

    private void disableEditText() {
        editText.setFocusable(false);
        editText.setHint(getString(R.string.insecure_keyboard_message));

        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                if (Utils.isUsingCustomInputMethod(getActivity())) {
                    final Intent i = new Intent();
                    i.setAction(Settings.ACTION_INPUT_METHOD_SETTINGS);
                    i.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                    i.addFlags(Intent.FLAG_ACTIVITY_NO_HISTORY);
                    i.addFlags(Intent.FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
                    getActivity().startActivity(i);
                }
            }
        }, 2000);

    }

    private void enableEditText() {
        editText.setFocusable(true);
        editText.setFocusableInTouchMode(true);
        editText.setHint("");
    }

    @Override
    public void onResume() {
        if (Utils.isUsingCustomInputMethod(getActivity())) {
            disableEditText();
        } else {
            enableEditText();
        }

        super.onResume();
    }

    public void showKeyBoard(boolean b) {
        if (b) {
            if (editText != null) {
                editText.dispatchTouchEvent(MotionEvent.obtain(SystemClock.uptimeMillis(), SystemClock.uptimeMillis(), MotionEvent.ACTION_DOWN, 0, 0, 0));
                editText.dispatchTouchEvent(MotionEvent.obtain(SystemClock.uptimeMillis(), SystemClock.uptimeMillis(), MotionEvent.ACTION_UP, 0, 0, 0));
            }
        } else {
            ((BreadWalletApp) getActivity().getApplication()).hideKeyboard(getActivity());
        }

    }
}
