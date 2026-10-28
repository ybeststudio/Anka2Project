# Paketleme öncesi ortak adımlar (GUI ve CLI aynı akışı kullanır):
#   1) bozuk ad temizliği (mod 'auto' ise)
#   2) property.xml güncellemesi (auto_property_xml) — değiştiyse hedef pack (root) listeye eklenir
from . import cleanup, config, propgen


def prepare_build(settings, names, log=print, cancel=None):
    """names: paketlenecek pack adları -> güncel ad listesi."""
    names = list(names)
    specs = config.discover(settings, log=log)
    if settings.get("broken_names") == "auto":
        found = cleanup.find([s for s in specs if s.name in names], copies=settings.get("broken_copies", False),
                             cancel=cancel)
        if found:
            n = cleanup.clean(found, specs, settings.get("broken_action", "quarantine"), log=log)
            log("temizlik: %d bozuk adlı dosya kaldırıldı" % n)
    if settings.get("auto_property_xml", True):
        changed, stats, tspec = propgen.update(settings, specs, log=log)
        if changed and tspec is not None and tspec.name not in names:
            names.append(tspec.name)
            log("property.xml değişti -> '%s' pack'i de paketlenecek" % tspec.name)
    return names
