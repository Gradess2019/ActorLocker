from typing import Optional, Union

import unreal


@unreal.uclass()
class UnlockAllActorsCommand(unreal.ActorLockerPythonCommand):

    current_map = unreal.uproperty(unreal.AssetData)
    current_id = unreal.uproperty(int)
    maps = unreal.uproperty(unreal.Array(unreal.AssetData))

    def load_map(self, map_asset: unreal.AssetData):
        print("Open map {0}".format(map_asset.package_name))
        subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
        return subsystem.load_level(str(map_asset.package_name))

    def request_unlock_actors(self):
        print("Waiting map {0} to be loaded...".format(self.current_map.package_name))
        self.set_editor_timer(self, "on_timer_finished", 1, False, -1)

    def execute(self):
        asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()
        all_assets = asset_registry.get_all_assets()
        self.maps = [asset for asset in all_assets if asset.asset_class_path.asset_name == "World" and str(asset.package_name).startswith("/Game")]

        self.current_id = 0
        self.process_next_map()

    @unreal.ufunction()
    def process_next_map(self):
        self.current_map = self.maps[self.current_id]
        if self.load_map(self.current_map):
            self.request_unlock_actors()

    @unreal.ufunction()
    def on_timer_finished(self):
        print("Map {0} is loaded. Unlocking actors...".format(self.current_map.package_name))
        unreal.ActorLockerCommandManager.unlock_all_objects()

        asset_subsystem = unreal.get_editor_subsystem(unreal.EditorAssetSubsystem)
        asset_subsystem.save_asset(str(self.current_map.package_name))

        self.current_id += 1
        if self.current_id < len(self.maps):
            print("Finished unlocking actors on map {0}.".format(self.current_map.package_name))
            self.process_next_map()
        else:
            print("Finished unlocking actors on all maps.")
            unreal.EditorDialog.show_message(unreal.Text("Actor Locker"), unreal.Text("Finished unlocking actors on all maps."), unreal.AppMsgType.OK)
            self.remove_object_from_root()


