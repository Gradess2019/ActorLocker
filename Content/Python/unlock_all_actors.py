import importlib
import unreal

import unlock_all_actors_command
importlib.reload(unlock_all_actors_command)
from unlock_all_actors_command import UnlockAllActorsCommand

if __name__ == '__main__':
    command = UnlockAllActorsCommand()
    command.add_object_to_root()
    command.execute()
