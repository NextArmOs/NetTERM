use eframe::egui;

pub struct PackageInfo {
    pub name: &'static str,
    pub command: &'static str,
    pub description: &'static str,
    pub status: &'static str,
}

pub fn get_installed_packages() -> Vec<PackageInfo> {
    vec![
        PackageInfo { name: "CodeEditor", command: ":edit / :editor", description: "Advanced text editor with lazy Emacs-style scrolling and multilang syntax", status: "Installed" },
        PackageInfo { name: "NetworkTerminal", command: ":net / :terminal", description: "Low-level system network diagnostic logs and socket terminal", status: "Installed" },
        PackageInfo { name: "CanvasPaint", command: ":paint / :canvas", description: "Multi-color responsive vector drawing board overlay", status: "Installed" },
        PackageInfo { name: "FetchTelemetry", command: ":fetch", description: "System specification and performance metrics telemetry grabber", status: "Installed" },
        PackageInfo { name: "MathCalculator", command: ":calc <expr>", description: "Algebraic formula calculator with algebraic parenthesis hierarchies", status: "Installed" },
        PackageInfo { name: "SettingsProfile", command: ":config / :settings", description: "Global interface properties, transparency controls, and language maps", status: "Installed" },
        PackageInfo { name: "HelpManual", command: ":help", description: "Core terminal application shell documentation matrix", status: "Installed" },
    ]
}

pub fn show(ui: &mut egui::Ui) {
    ui.heading("📦 NetTERM_PKGS System Package Manager");
    ui.add_space(10.0);
    ui.separator();
    ui.add_space(15.0);

    let packages = get_installed_packages();
    let total_pkgs = packages.len();

    ui.vertical(|ui| {
        ui.horizontal(|ui| {
            ui.label(egui::RichText::new("SYSTEM CORE STATUS:").strong());
            ui.colored_label(egui::Color32::LIGHT_GREEN, "OPTIMIZED");
            ui.separator();
            ui.label(format!("Total Registered Packages: {}", total_pkgs));
        });
        ui.add_space(15.0);

        egui::ScrollArea::vertical().show(ui, |ui| {
            egui::Grid::new("pkgs_grid")
                .num_columns(4)
                .spacing([30.0, 12.0])
                .striped(true)
                .show(ui, |ui| {
                    ui.label(egui::RichText::new("PACKAGE NAME").strong().color(egui::Color32::from_hex("#ff757f").unwrap()));
                    ui.label(egui::RichText::new("SHELL COMMAND").strong().color(egui::Color32::from_hex("#00ffff").unwrap()));
                    ui.label(egui::RichText::new("DESCRIPTION SUMMARY").strong().color(egui::Color32::from_gray(160)));
                    ui.label(egui::RichText::new("STATUS").strong().color(egui::Color32::LIGHT_GREEN));
                    ui.end_row();

                    for pkg in &packages {
                        ui.label(pkg.name);
                        ui.label(egui::RichText::new(pkg.command).monospace());
                        ui.label(pkg.description);
                        ui.colored_label(egui::Color32::LIGHT_GREEN, pkg.status);
                        ui.end_row();
                    }
                });
        });
    });
}
