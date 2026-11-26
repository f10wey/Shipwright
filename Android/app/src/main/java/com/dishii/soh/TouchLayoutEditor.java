package com.dishii.soh;

import android.app.Activity;
import android.content.SharedPreferences;
import android.graphics.Color;
import android.graphics.Rect;
import android.graphics.drawable.GradientDrawable;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.SeekBar;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;

/**
 * Lets the player move, resize, hide and fade the on-screen controls.
 *
 * Positions are stored as translation offsets from the default layout and sizes as a scale factor,
 * so the original constraint layout keeps working and "Reset" simply clears the offsets.
 * Long-press the eye (toggle) button to open the editor.
 */
public class TouchLayoutEditor {

    /** A movable unit: a single button, the joystick, or the C-button cross (several views moved together). */
    private static class Control {
        final String key;
        final String title;
        final View[] views;
        final boolean hiddenByDefault;

        Control(String key, String title, boolean hiddenByDefault, View... views) {
            this.key = key;
            this.title = title;
            this.hiddenByDefault = hiddenByDefault;
            this.views = views;
        }
    }

    private static final String PREFIX = "touchLayout.";
    private static final float DEFAULT_OPACITY = 0.75f;
    private static final float MIN_SCALE = 0.5f;
    private static final float MAX_SCALE = 2.5f;

    private final Activity activity;
    private final ViewGroup overlayRoot;
    private final ViewGroup buttonGroup;
    private final SharedPreferences prefs;
    private final List<Control> controls = new ArrayList<>();

    private boolean editing = false;
    private boolean ocarinaBoost = false;
    private static final float OCARINA_SCALE = 1.5f;

    /** Enlarges the note buttons (A and the C buttons) while the ocarina is being played. */
    public void setOcarinaBoost(boolean on) {
        if (on != ocarinaBoost) {
            ocarinaBoost = on;
            apply();
        }
    }
    private Control selected = null;
    private FrameLayout editLayer;
    private TextView selectedLabel;
    private SeekBar sizeBar;
    private Button hideButton;

    public TouchLayoutEditor(Activity activity, ViewGroup overlayRoot, ViewGroup buttonGroup, SharedPreferences prefs) {
        this.activity = activity;
        this.overlayRoot = overlayRoot;
        this.buttonGroup = buttonGroup;
        this.prefs = prefs;
        importFromFileIfEmpty();
    }

    // ---------------------------------------------------------------- layout file
    // SharedPreferences are wiped when the app is uninstalled, so the layout is also kept in
    // SOH/touch_layout.txt next to the saves and restored from there after a reinstall.

    private static java.io.File layoutFile() {
        return new java.io.File(new java.io.File(android.os.Environment.getExternalStorageDirectory(), "SOH"),
                "touch_layout.txt");
    }

    private void importFromFileIfEmpty() {
        java.io.File f = layoutFile();
        for (String k : prefs.getAll().keySet()) {
            if (k.startsWith(PREFIX)) {
                // Layout from before this version: put it in the file so it survives reinstalls
                if (!f.exists()) {
                    exportToFile();
                }
                return;
            }
        }
        if (!f.exists()) {
            return;
        }
        try (java.io.BufferedReader r = new java.io.BufferedReader(new java.io.FileReader(f))) {
            SharedPreferences.Editor ed = prefs.edit();
            String line;
            while ((line = r.readLine()) != null) {
                // key=f:1.5 or key=b:true
                int eq = line.indexOf('=');
                if (eq <= 0 || line.length() < eq + 3) {
                    continue;
                }
                String key = line.substring(0, eq);
                String type = line.substring(eq + 1, eq + 2);
                String value = line.substring(eq + 3);
                if (!key.startsWith(PREFIX)) {
                    continue;
                }
                if (type.equals("f")) {
                    ed.putFloat(key, Float.parseFloat(value));
                } else if (type.equals("b")) {
                    ed.putBoolean(key, Boolean.parseBoolean(value));
                }
            }
            ed.apply();
        } catch (Exception e) {
            android.util.Log.w("TouchLayout", "Cannot read " + f, e);
        }
    }

    private void exportToFile() {
        StringBuilder sb = new StringBuilder();
        for (java.util.Map.Entry<String, ?> e : prefs.getAll().entrySet()) {
            if (!e.getKey().startsWith(PREFIX)) {
                continue;
            }
            Object v = e.getValue();
            if (v instanceof Float) {
                sb.append(e.getKey()).append("=f:").append(v).append('\n');
            } else if (v instanceof Boolean) {
                sb.append(e.getKey()).append("=b:").append(v).append('\n');
            }
        }
        java.io.File f = layoutFile();
        try {
            f.getParentFile().mkdirs();
            try (java.io.FileWriter w = new java.io.FileWriter(f)) {
                w.write(sb.toString());
            }
        } catch (Exception e) {
            android.util.Log.w("TouchLayout", "Cannot write " + f, e);
        }
    }

    public void addControl(String key, String title, boolean hiddenByDefault, View... views) {
        controls.add(new Control(key, title, hiddenByDefault, views));
    }

    // ---------------------------------------------------------------- saved layout

    private float getF(String name, float def) {
        return prefs.getFloat(PREFIX + name, def);
    }

    private boolean isHidden(Control c) {
        return prefs.getBoolean(PREFIX + c.key + ".hidden", c.hiddenByDefault);
    }

    private float opacity() {
        return getF("opacity", DEFAULT_OPACITY);
    }

    /** Applies the saved positions, sizes, visibility and opacity to the views. */
    public void apply() {
        if (editing) {
            exportToFile();
        }
        float alpha = opacity();
        for (Control c : controls) {
            float dx = getF(c.key + ".dx", 0f);
            float dy = getF(c.key + ".dy", 0f);
            float scale = getF(c.key + ".scale", 1f);
            if (ocarinaBoost && !editing && (c.key.equals("c") || c.key.equals("a"))) {
                scale *= OCARINA_SCALE;
            }
            boolean hidden = isHidden(c);
            // Controls made of several views (the C buttons) scale around the first view's center,
            // so the group grows as a whole instead of every button growing in place
            View first = c.views[0];
            float cx = first.getLeft() + first.getWidth() / 2f;
            float cy = first.getTop() + first.getHeight() / 2f;
            for (View v : c.views) {
                if (c.views.length > 1 && v.getWidth() > 0) {
                    v.setPivotX(cx - v.getLeft());
                    v.setPivotY(cy - v.getTop());
                }
                v.setTranslationX(dx);
                v.setTranslationY(dy);
                v.setScaleX(scale);
                v.setScaleY(scale);
                if (editing) {
                    // Keep hidden controls visible (faded) in the editor so they can be shown again
                    v.setVisibility(View.VISIBLE);
                    v.setAlpha(hidden ? 0.2f : Math.max(alpha, 0.5f));
                } else {
                    v.setVisibility(hidden ? View.GONE : View.VISIBLE);
                    v.setAlpha(alpha);
                }
            }
        }
    }

    private void save(Control c, float dx, float dy) {
        prefs.edit().putFloat(PREFIX + c.key + ".dx", dx).putFloat(PREFIX + c.key + ".dy", dy).apply();
    }

    // ---------------------------------------------------------------- editor UI

    public boolean isEditing() {
        return editing;
    }

    public void enterEditMode() {
        if (editing) {
            return;
        }
        editing = true;
        buttonGroup.setVisibility(View.VISIBLE);
        buildEditLayer();
        apply();
        select(null);
    }

    public void exitEditMode() {
        if (!editing) {
            return;
        }
        editing = false;
        exportToFile();
        overlayRoot.removeView(editLayer);
        editLayer = null;
        selected = null;
        apply();
    }

    private int dp(float value) {
        return Math.round(value * activity.getResources().getDisplayMetrics().density);
    }

    private Button panelButton(String text, View.OnClickListener listener) {
        Button b = new Button(activity);
        b.setText(text);
        b.setAllCaps(false);
        b.setTextSize(13);
        b.setTextColor(Color.WHITE);
        GradientDrawable bg = new GradientDrawable();
        bg.setColor(0xFF3A3A3A);
        bg.setCornerRadius(dp(8));
        b.setBackground(bg);
        b.setPadding(dp(10), 0, dp(10), 0);
        b.setMinHeight(dp(36));
        b.setMinimumHeight(dp(36));
        b.setOnClickListener(listener);
        LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(
                ViewGroup.LayoutParams.WRAP_CONTENT, dp(36));
        lp.setMargins(dp(4), dp(4), dp(4), dp(4));
        b.setLayoutParams(lp);
        return b;
    }

    private TextView label(String text) {
        TextView t = new TextView(activity);
        t.setText(text);
        t.setTextColor(Color.WHITE);
        t.setTextSize(13);
        t.setPadding(dp(6), 0, dp(6), 0);
        return t;
    }

    private void buildEditLayer() {
        editLayer = new FrameLayout(activity);
        editLayer.setBackgroundColor(0x55000000);
        editLayer.setLayoutParams(new FrameLayout.LayoutParams(
                ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
        editLayer.setOnTouchListener(new DragHandler());

        // Control panel at the top centre
        LinearLayout panel = new LinearLayout(activity);
        panel.setOrientation(LinearLayout.VERTICAL);
        panel.setPadding(dp(10), dp(6), dp(10), dp(6));
        GradientDrawable panelBg = new GradientDrawable();
        panelBg.setColor(0xE6202020);
        panelBg.setCornerRadius(dp(12));
        panel.setBackground(panelBg);
        // Swallow touches on the panel so they do not start a drag
        panel.setOnTouchListener((v, e) -> true);

        selectedLabel = label("");
        selectedLabel.setGravity(Gravity.CENTER);
        panel.addView(selectedLabel);

        LinearLayout row1 = new LinearLayout(activity);
        row1.setGravity(Gravity.CENTER_VERTICAL);
        row1.addView(label("Прозрачность"));
        SeekBar opacityBar = new SeekBar(activity);
        opacityBar.setMax(90);
        opacityBar.setProgress(Math.round((opacity() - 0.1f) * 100f));
        opacityBar.setOnSeekBarChangeListener(new SimpleSeek(progress -> {
            prefs.edit().putFloat(PREFIX + "opacity", 0.1f + progress / 100f).apply();
            apply();
        }));
        row1.addView(opacityBar, new LinearLayout.LayoutParams(dp(180), ViewGroup.LayoutParams.WRAP_CONTENT));
        row1.addView(label("Размер"));
        sizeBar = new SeekBar(activity);
        sizeBar.setMax(Math.round((MAX_SCALE - MIN_SCALE) * 100f));
        sizeBar.setOnSeekBarChangeListener(new SimpleSeek(progress -> {
            if (selected != null) {
                prefs.edit().putFloat(PREFIX + selected.key + ".scale", MIN_SCALE + progress / 100f).apply();
                apply();
            }
        }));
        row1.addView(sizeBar, new LinearLayout.LayoutParams(dp(180), ViewGroup.LayoutParams.WRAP_CONTENT));
        panel.addView(row1);

        LinearLayout row2 = new LinearLayout(activity);
        row2.setGravity(Gravity.CENTER);
        hideButton = panelButton("Скрыть", v -> {
            if (selected != null) {
                prefs.edit().putBoolean(PREFIX + selected.key + ".hidden", !isHidden(selected)).apply();
                select(selected);
                apply();
            }
        });
        row2.addView(hideButton);
        row2.addView(panelButton("Сбросить всё", v -> {
            SharedPreferences.Editor ed = prefs.edit();
            for (String k : prefs.getAll().keySet()) {
                if (k.startsWith(PREFIX)) {
                    ed.remove(k);
                }
            }
            ed.apply();
            opacityBar.setProgress(Math.round((DEFAULT_OPACITY - 0.1f) * 100f));
            select(null);
            apply();
        }));
        row2.addView(panelButton("Готово", v -> exitEditMode()));
        panel.addView(row2);

        FrameLayout.LayoutParams panelLp = new FrameLayout.LayoutParams(
                ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT,
                Gravity.TOP | Gravity.CENTER_HORIZONTAL);
        panelLp.topMargin = dp(8);
        editLayer.addView(panel, panelLp);

        overlayRoot.addView(editLayer);
    }

    private void select(Control c) {
        selected = c;
        boolean has = c != null;
        sizeBar.setEnabled(has);
        hideButton.setEnabled(has);
        hideButton.setAlpha(has ? 1f : 0.4f);
        if (has) {
            selectedLabel.setText("Выбрано: " + c.title + "  ·  тяни пальцем, чтобы переместить");
            sizeBar.setProgress(Math.round((getF(c.key + ".scale", 1f) - MIN_SCALE) * 100f));
            hideButton.setText(isHidden(c) ? "Показать" : "Скрыть");
        } else {
            selectedLabel.setText("Нажми на кнопку, чтобы выбрать её, и тяни, чтобы переместить");
            hideButton.setText("Скрыть");
        }
    }

    /** Finds the control under a point (in overlay coordinates), taking moved/scaled views into account. */
    private Control findControlAt(float x, float y) {
        int[] origin = new int[2];
        overlayRoot.getLocationOnScreen(origin);
        Rect r = new Rect();
        int[] loc = new int[2];
        for (int i = controls.size() - 1; i >= 0; i--) {
            Control c = controls.get(i);
            for (View v : c.views) {
                if (v.getWidth() == 0) {
                    continue;
                }
                v.getLocationOnScreen(loc);
                float w = v.getWidth() * v.getScaleX();
                float h = v.getHeight() * v.getScaleY();
                r.set(loc[0] - origin[0], loc[1] - origin[1],
                        Math.round(loc[0] - origin[0] + w), Math.round(loc[1] - origin[1] + h));
                // Make tiny buttons (C-buttons) easier to grab
                r.inset(-dp(8), -dp(8));
                if (r.contains(Math.round(x), Math.round(y))) {
                    return c;
                }
            }
        }
        return null;
    }

    private class DragHandler implements View.OnTouchListener {
        private float lastX, lastY;
        private Control dragging;

        @Override
        public boolean onTouch(View v, MotionEvent e) {
            switch (e.getActionMasked()) {
                case MotionEvent.ACTION_DOWN:
                    dragging = findControlAt(e.getX(), e.getY());
                    select(dragging);
                    lastX = e.getX();
                    lastY = e.getY();
                    return true;
                case MotionEvent.ACTION_MOVE:
                    if (dragging != null) {
                        float dx = getF(dragging.key + ".dx", 0f) + (e.getX() - lastX);
                        float dy = getF(dragging.key + ".dy", 0f) + (e.getY() - lastY);
                        save(dragging, dx, dy);
                        lastX = e.getX();
                        lastY = e.getY();
                        apply();
                    }
                    return true;
                case MotionEvent.ACTION_UP:
                case MotionEvent.ACTION_CANCEL:
                    dragging = null;
                    return true;
            }
            return true;
        }
    }

    private interface ProgressCallback {
        void onProgress(int progress);
    }

    private static class SimpleSeek implements SeekBar.OnSeekBarChangeListener {
        private final ProgressCallback cb;

        SimpleSeek(ProgressCallback cb) {
            this.cb = cb;
        }

        @Override
        public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
            if (fromUser) {
                cb.onProgress(progress);
            }
        }

        @Override
        public void onStartTrackingTouch(SeekBar seekBar) {
        }

        @Override
        public void onStopTrackingTouch(SeekBar seekBar) {
        }
    }
}
