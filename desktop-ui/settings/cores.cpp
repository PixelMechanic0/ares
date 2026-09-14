auto CoreSettings::construct() -> void {
  setCollapsible();
  setVisible(false);

  settingsHint.setText("Note: Settings changes require a game reload to take effect").setFont(Font().setSize(7.0)).setForegroundColor(SystemColor::Sublabel);

  nintendo64SettingsLabel.setText("Nintendo 64 Settings").setFont(Font().setBold());

  nintendo64ExpansionPakOption.setText("4MB Expansion Pak").setChecked(settings.nintendo64.expansionPak).onToggle([&] {
    settings.nintendo64.expansionPak = nintendo64ExpansionPakOption.checked();
  });
  nintendo64ExpansionPakLayout.setAlignment(1).setPadding(12_sx, 0);
    nintendo64ExpansionPakHint.setText("Enable/Disable the 4MB Expansion Pak").setFont(Font().setSize(7.0)).setForegroundColor(SystemColor::Sublabel);

  for(auto& opt : array<string[4]>{"32KiB (Default)", "128KiB (Datel 1Meg)", "512KiB (Datel 4Meg)", "1984KiB (Maximum)"}) {
    ComboButtonItem item{&nintendo64ControllerPakBankOption};
    item.setText(opt);
    if (opt == settings.nintendo64.controllerPakBankString) {
      item.setSelected();

      if (opt == "32KiB (Default)") {
        settings.nintendo64.controllerPakBankCount = 1;
      } else if (opt == "128KiB (Datel 1Meg)") {
        settings.nintendo64.controllerPakBankCount = 4;
      } else if (opt == "512KiB (Datel 4Meg)") {
        settings.nintendo64.controllerPakBankCount = 16;
      } else if (opt == "1984KiB (Maximum)") {
        settings.nintendo64.controllerPakBankCount = 62;
      }
    }
  }
  nintendo64ControllerPakBankOption.onChange([&] {
    auto idx = nintendo64ControllerPakBankOption.selected();
    auto value = idx.text();
    if (value != settings.nintendo64.controllerPakBankString) {
      settings.nintendo64.controllerPakBankString = value;
      
      if (value == "32KiB (Default)") {
        settings.nintendo64.controllerPakBankCount = 1;
      } else if (value == "128KiB (Datel 1Meg)") {
        settings.nintendo64.controllerPakBankCount = 4;
      } else if (value == "512KiB (Datel 4Meg)") {
        settings.nintendo64.controllerPakBankCount = 16;
      } else if (value == "1984KiB (Maximum)") {
        settings.nintendo64.controllerPakBankCount = 62;
      }
    }
  });
  nintendo64ControllerPakBankLayout.setAlignment(0.5).setPadding(12_sx, 0);
    nintendo64ControllerPakBankLabel.setText("Controller Pak Size:");
    nintendo64ControllerPakBankHint.setText("Sets the size of a newly created Controller Pak's available memory").setFont(Font().setSize(7.0)).setForegroundColor(SystemColor::Sublabel);

  renderQualityLayout.setPadding(12_sx, 0);

  renderQuality1x.setText("1x Native").onActivate([&] {
    settings.nintendo64.quality = "SD";
  });
  renderQuality2x.setText("2x Native").onActivate([&] {
    settings.nintendo64.quality = "HD";
  });
  if(settings.nintendo64.quality == "SD") renderQuality1x.setChecked();
  if(settings.nintendo64.quality != "SD") {
    settings.nintendo64.quality = "HD";
    renderQuality2x.setChecked();
  }

  disableVIDitherFilterOption.setText("Disable VI Dither Filter").setChecked(settings.nintendo64.disableVIDitherFilter).onToggle([&] {
    settings.nintendo64.disableVIDitherFilter = disableVIDitherFilterOption.checked();
  });
  disableVIDitherFilterLayout.setAlignment(1).setPadding(12_sx, 0);

  disableVIDivotFilterOption.setText("Disable VI Divot Filter").setChecked(settings.nintendo64.disableVIDivotFilter).onToggle([&] {
    settings.nintendo64.disableVIDivotFilter = disableVIDivotFilterOption.checked();
  });
  disableVIDivotFilterLayout.setAlignment(1).setPadding(12_sx, 0);

  disableVIGammaDitherOption.setText("Disable VI Gamma Dither").setChecked(settings.nintendo64.disableVIGammaDither).onToggle([&] {
    settings.nintendo64.disableVIGammaDither = disableVIGammaDitherOption.checked();
  });
  disableVIGammaDitherLayout.setAlignment(1).setPadding(12_sx, 0);

  disableVIAntiAliasingOption.setText("Disable VI Anti-Aliasing").setChecked(settings.nintendo64.disableVIAntiAliasing).onToggle([&] {
    settings.nintendo64.disableVIAntiAliasing = disableVIAntiAliasingOption.checked();
  });
  disableVIAntiAliasingLayout.setAlignment(1).setPadding(12_sx, 0);

  gameBoyAdvanceSettingsLabel.setText("Game Boy Advance Settings").setFont(Font().setBold());
  gameBoyPlayerOption.setText("Game Boy Player").setChecked(settings.gameBoyAdvance.player).onToggle([&] {
    settings.gameBoyAdvance.player = gameBoyPlayerOption.checked();
  });
  gameBoyPlayerLayout.setAlignment(1).setPadding(12_sx, 0);
    gameBoyPlayerHint.setText("Enable/Disable Game Boy Player rumble").setFont(Font().setSize(7.0)).setForegroundColor(SystemColor::Sublabel);

  superFamicomSettingsLabel.setText("Super Famicom Settings").setFont(Font().setBold());
  superFamicomDeepBlackBoostOption.setText("Deep Black Boost").setChecked(settings.superFamicom.deepBlackBoost).onToggle([&] {
    Program::Guard guard;
    settings.superFamicom.deepBlackBoost = superFamicomDeepBlackBoostOption.checked();
    if(emulator) emulator->setBoolean("Deep Black Boost", settings.superFamicom.deepBlackBoost);
  });
  superFamicomDeepBlackBoostLayout.setAlignment(1).setPadding(12_sx, 0);
  superFamicomDeepBlackBoostHint.setText("Applies a gamma ramp to crush black levels").setFont(Font().setSize(7.0)).setForegroundColor(SystemColor::Sublabel);

  megaDriveSettingsLabel.setText("Mega Drive Settings").setFont(Font().setBold());
  megaDriveTmssOption.setText("TMSS Boot Rom").setChecked(settings.megadrive.tmss).onToggle([&] {
    settings.megadrive.tmss = megaDriveTmssOption.checked();
  });
  megaDriveTmssLayout.setAlignment(1).setPadding(12_sx, 0);
    megaDriveTmssHint.setText("Enable/Disable the TMSS Boot Rom at system initialization").setFont(Font().setSize(7.0)).setForegroundColor(SystemColor::Sublabel);
}
