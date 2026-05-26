import numpy as np
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec

MU = 3.986004418e14
R_EARTH = 6371000.0

def compute_apogee_and_energy(rx, ry, rz, vx, vy, vz):
    r_vec = np.array([rx, ry, rz])
    v_vec = np.array([vx, vy, vz])
    r = np.linalg.norm(r_vec)
    v = np.linalg.norm(v_vec)

    energy = v**2 / 2.0 - MU / r

    if energy >= 0:
        return None, energy, r

    e_vec = (v**2/MU - 1.0/r)*r_vec - (np.dot(r_vec, v_vec)/MU)*v_vec
    e = np.linalg.norm(e_vec)
    a = -MU / (2.0 * energy)
    apogee = a * (1.0 + e) - R_EARTH

    return apogee, energy, r


def parse_file(filepath):
    times, apogees, energies, altitudes = [], [], [], []

    with open(filepath, 'r') as f:
        raw = f.read()

    tokens = raw.split()
    numbers = []
    for tok in tokens:
        try:
            numbers.append(float(tok))
        except ValueError:
            continue

    # формат: t rx ry rz vx vy vz m mx my mz earth_angle
    i = 0
    while i + 11 < len(numbers):
        t, rx, ry, rz, vx, vy, vz, m = numbers[i:i+8]
        i += 12  # пропускаем mx my mz earth_angle

        apogee, energy, r = compute_apogee_and_energy(rx, ry, rz, vx, vy, vz)

        times.append(t)
        energies.append(energy)
        altitudes.append((r - R_EARTH) / 1000.0)
        apogees.append(apogee / 1000.0 if apogee is not None else None)

    return (np.array(times),
            np.array(apogees, dtype=object),
            np.array(energies),
            np.array(altitudes))


def main(filepath):
    times, apogees, energies, altitudes = parse_file(filepath)

    # Маска только для эллиптических точек
    elliptic = np.array([a is not None for a in apogees])
    t_ell   = times[elliptic]
    apo_ell = np.array(apogees[elliptic], dtype=float)

    fig = plt.figure(figsize=(14, 10))
    fig.suptitle('Диагностика траектории', fontsize=14, fontweight='bold')
    gs = gridspec.GridSpec(3, 1, hspace=0.45)

    # --- График 1: Апогей ---
    ax1 = fig.add_subplot(gs[0])
    ax1.plot(t_ell, apo_ell, color='royalblue', linewidth=1.2, label='Апогей')
    ax1.set_ylabel('Апогей, км')
    ax1.set_title('Текущий апогей (только эллиптические точки)')
    ax1.grid(True, alpha=0.35)
    ax1.legend(fontsize=8)

    vline1 = ax1.axvline(x=times[0], color='tomato', linewidth=1.0, linestyle='--', alpha=0.8)
    annot1 = ax1.annotate('', xy=(0,0), xytext=(12,12), textcoords='offset points',
                          bbox=dict(boxstyle='round,pad=0.3', fc='white', ec='gray', alpha=0.9),
                          arrowprops=dict(arrowstyle='->', color='gray'), fontsize=8)
    annot1.set_visible(False)

    # --- График 2: Орбитальная энергия ---
    ax2 = fig.add_subplot(gs[1])
    ax2.plot(times, energies / 1e6, color='darkorange', linewidth=1.2, label='Энергия')
    ax2.axhline(0, color='red', linewidth=0.8, linestyle='--', alpha=0.6, label='E=0 (парабола)')
    ax2.set_ylabel('Энергия, МДж/кг')
    ax2.set_title('О')
    ax2.grid(True, alpha=0.35)
    ax2.legend(fontsize=8)

    vline2 = ax2.axvline(x=times[0], color='tomato', linewidth=1.0, linestyle='--', alpha=0.8)
    annot2 = ax2.annotate('', xy=(0,0), xytext=(12,12), textcoords='offset points',
                          bbox=dict(boxstyle='round,pad=0.3', fc='white', ec='gray', alpha=0.9),
                          arrowprops=dict(arrowstyle='->', color='gray'), fontsize=8)
    annot2.set_visible(False)

    # --- График 3: Высота ---
    ax3 = fig.add_subplot(gs[2])
    ax3.plot(times, altitudes, color='seagreen', linewidth=1.2, label='Высота')
    ax3.axhline(384400, color='gray', linewidth=0.8, linestyle=':', alpha=0.7, label='Орбита Луны')
    ax3.set_ylabel('Высота, км')
    ax3.set_xlabel('Время, с')
    ax3.set_title('Высота над поверхностью Земли')
    ax3.grid(True, alpha=0.35)
    ax3.legend(fontsize=8)

    vline3 = ax3.axvline(x=times[0], color='tomato', linewidth=1.0, linestyle='--', alpha=0.8)
    annot3 = ax3.annotate('', xy=(0,0), xytext=(12,12), textcoords='offset points',
                          bbox=dict(boxstyle='round,pad=0.3', fc='white', ec='gray', alpha=0.9),
                          arrowprops=dict(arrowstyle='->', color='gray'), fontsize=8)
    annot3.set_visible(False)

    axes   = [ax1, ax2, ax3]
    vlines = [vline1, vline2, vline3]
    annots = [annot1, annot2, annot3]

    def on_move(event):
        if event.inaxes not in axes:
            for a in annots: a.set_visible(False)
            for v in vlines: v.set_visible(False)
            fig.canvas.draw_idle()
            return

        idx = np.searchsorted(times, event.xdata)
        idx = np.clip(idx, 0, len(times) - 1)
        tx = times[idx]

        for v in vlines:
            v.set_xdata([tx])
            v.set_visible(True)

        # Аннотация только в том графике где курсор
        for i, (ax, ann) in enumerate(zip(axes, annots)):
            if event.inaxes == ax:
                if ax == ax1:
                    # Найти ближайшую эллиптическую точку
                    if elliptic.any():
                        idx_e = np.searchsorted(t_ell, tx)
                        idx_e = np.clip(idx_e, 0, len(t_ell)-1)
                        val = apo_ell[idx_e]
                        ann.xy = (t_ell[idx_e], val)
                        ann.set_text(f't = {tx:.0f} с\nАпогей = {val:.0f} км')
                elif ax == ax2:
                    val = energies[idx] / 1e6
                    ann.xy = (tx, val)
                    ann.set_text(f't = {tx:.0f} с\nE = {val:.4f} МДж/кг')
                elif ax == ax3:
                    val = altitudes[idx]
                    ann.xy = (tx, val)
                    ann.set_text(f't = {tx:.0f} с\nВысота = {val:.0f} км')
                ann.set_visible(True)
            else:
                ann.set_visible(False)

        fig.canvas.draw_idle()

    fig.canvas.mpl_connect('motion_notify_event', on_move)

    plt.savefig('trajectory_diag.png', dpi=150)
    plt.show()

    print(f"Точек: {len(times)}")
    print(f"Эллиптических: {elliptic.sum()} ({100*elliptic.mean():.1f}%)")
    print(f"Финальная высота:  {altitudes[-1]:.0f} км")
    print(f"Финальная энергия: {energies[-1]/1e6:.4f} МДж/кг")
    if elliptic.any():
        print(f"Финальный апогей:  {apo_ell[-1]:.0f} км")


if __name__ == '__main__':
    import sys
    path = sys.argv[1] if len(sys.argv) > 1 else 'trajectory.txt'
    main(path)