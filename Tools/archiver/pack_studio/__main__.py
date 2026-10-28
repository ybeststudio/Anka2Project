import multiprocessing as mp
import sys

if __name__ == "__main__":
    mp.freeze_support()
    if len(sys.argv) > 1:
        from pack_studio.cli import main
        sys.exit(main())
    from pack_studio.gui import main
    sys.exit(main())
