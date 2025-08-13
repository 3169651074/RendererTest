import numpy as np
import matplotlib.pyplot as plt
#from mpl_toolkits.mplot3d import Axes3D
from matplotlib.widgets import Slider

def read_coordinates(filename):
    """
    读取坐标文件，返回xyz坐标数组

    Parameters:
    filename (str): 坐标文件路径

    Returns:
    tuple: (x_coords, y_coords, z_coords)
    """
    coordinates = []

    try:
        with open(filename, 'r', encoding='utf-8') as file:
            for line_num, line in enumerate(file, 1):
                line = line.strip()
                if not line:  # 跳过空行
                    continue

                try:
                    # 分割每行的三个浮点数
                    parts = line.split()
                    if len(parts) != 3:
                        print(f"Warning: Line {line_num} has incorrect format, skipping: {line}")
                        continue

                    x, y, z = map(float, parts)
                    coordinates.append((x, y, z))

                except ValueError as e:
                    print(f"Warning: Failed to convert data on line {line_num}, skipping: {line}")
                    continue

    except FileNotFoundError:
        print(f"Error: Could not find file '{filename}'")
        return None, None, None
    except Exception as e:
        print(f"Error: Exception occurred while reading file: {e}")
        return None, None, None

    if not coordinates:
        print("Warning: No valid coordinate data found")
        return None, None, None

    # 转换为numpy数组并分离xyz坐标
    coords_array = np.array(coordinates)
    x_coords = coords_array[:, 0]
    y_coords = coords_array[:, 1]
    z_coords = coords_array[:, 2]

    print(f"Successfully read {len(coordinates)} coordinate points")
    return x_coords, y_coords, z_coords

def visualize_3d_points_with_slider(x_coords, y_coords, z_coords,
                                    initial_size=80, alpha=0.7, colormap='viridis'):
    """
    创建带滑动条的3D散点图可视化坐标点

    Parameters:
    x_coords, y_coords, z_coords: 坐标数组
    initial_size: 初始点大小
    alpha: 透明度
    colormap: 颜色映射
    """
    # 创建图形和子图布局
    fig = plt.figure(figsize=(14, 10))

    # 为3D图预留空间（上方大部分区域）
    ax = fig.add_subplot(111, projection='3d')

    # 调整子图位置，为滑动条留出空间
    plt.subplots_adjust(bottom=0.15)

    # 创建初始散点图
    scatter = ax.scatter(x_coords, y_coords, z_coords,
                         c=z_coords, s=initial_size, alpha=alpha,
                         cmap=colormap, edgecolors='black', linewidth=0.5)

    # 设置标签和标题
    ax.set_xlabel('X Coordinate', fontsize=12)
    ax.set_ylabel('Y Coordinate', fontsize=12)
    ax.set_zlabel('Z Coordinate', fontsize=12)
    ax.set_title('3D Coordinate Points Visualization (Interactive)', fontsize=14, fontweight='bold')

    # 添加颜色条
    cbar = plt.colorbar(scatter, ax=ax, shrink=0.5, aspect=20)
    cbar.set_label('Z Value', fontsize=10)

    # 显示坐标范围信息
    print(f"Coordinate ranges:")
    print(f"X: [{x_coords.min():.6f}, {x_coords.max():.6f}]")
    print(f"Y: [{y_coords.min():.6f}, {y_coords.max():.6f}]")
    print(f"Z: [{z_coords.min():.6f}, {z_coords.max():.6f}]")

    # 创建滑动条
    # 滑动条的位置 [left, bottom, width, height]
    ax_slider = plt.axes([0.2, 0.05, 0.6, 0.03])
    slider = Slider(ax_slider, 'Point Size', 1, 200, valinit=initial_size, valfmt='%d')

    # 定义滑动条回调函数
    def update_size(val):
        """更新点的大小"""
        new_size = slider.val
        # 移除旧的散点图
        ax.collections.clear()
        # 创建新的散点图
        new_scatter = ax.scatter(x_coords, y_coords, z_coords,
                                 c=z_coords, s=new_size, alpha=alpha,
                                 cmap=colormap, edgecolors='black', linewidth=0.5)
        # 重新绘制
        fig.canvas.draw()

    # 连接滑动条和回调函数
    slider.on_changed(update_size)

    # 添加说明文字
    fig.text(0.02, 0.95, 'Instructions:\n• Drag to rotate view\n• Scroll to zoom\n• Use slider to adjust point size',
             fontsize=10, verticalalignment='top', bbox=dict(boxstyle='round', facecolor='lightgray', alpha=0.8))

    # 优化显示
    plt.tight_layout()

    return fig, ax, slider

def create_multiple_views_with_sliders(x_coords, y_coords, z_coords):
    """
    创建带滑动条的多视角显示
    """
    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(16, 12))
    fig.suptitle('Multi-View Coordinate Visualization (Interactive)', fontsize=16, fontweight='bold')

    # 调整子图布局，为滑动条留出空间
    plt.subplots_adjust(bottom=0.15, hspace=0.3, wspace=0.3)

    initial_size = 80
    alpha = 0.7

    # 3D视图 - 转换为2D投影以支持滑动条
    scatter1 = ax1.scatter(x_coords, y_coords, c=z_coords, cmap='viridis', alpha=alpha, s=initial_size)
    ax1.set_title('XY Plane View (Colored by Z)')
    ax1.set_xlabel('X Coordinate')
    ax1.set_ylabel('Y Coordinate')
    ax1.grid(True, alpha=0.3)

    # XY平面投影
    scatter2 = ax2.scatter(x_coords, z_coords, c=y_coords, cmap='plasma', alpha=alpha, s=initial_size)
    ax2.set_title('XZ Plane View (Colored by Y)')
    ax2.set_xlabel('X Coordinate')
    ax2.set_ylabel('Z Coordinate')
    ax2.grid(True, alpha=0.3)

    # XZ平面投影
    scatter3 = ax3.scatter(y_coords, z_coords, c=x_coords, cmap='coolwarm', alpha=alpha, s=initial_size)
    ax3.set_title('YZ Plane View (Colored by X)')
    ax3.set_xlabel('Y Coordinate')
    ax3.set_ylabel('Z Coordinate')
    ax3.grid(True, alpha=0.3)

    # 第四个子图显示统计信息
    ax4.axis('off')
    stats_text = f"""Coordinate Statistics:
    
Number of points: {len(x_coords)}

X Range: [{x_coords.min():.3f}, {x_coords.max():.3f}]
Y Range: [{y_coords.min():.3f}, {y_coords.max():.3f}]
Z Range: [{z_coords.min():.3f}, {z_coords.max():.3f}]

X Mean: {x_coords.mean():.3f}
Y Mean: {y_coords.mean():.3f}
Z Mean: {z_coords.mean():.3f}

X Std: {x_coords.std():.3f}
Y Std: {y_coords.std():.3f}
Z Std: {z_coords.std():.3f}"""

    ax4.text(0.1, 0.9, stats_text, fontsize=11, verticalalignment='top',
             bbox=dict(boxstyle='round', facecolor='lightblue', alpha=0.8))
    ax4.set_title('Data Statistics')

    # 创建滑动条
    ax_slider = plt.axes([0.2, 0.05, 0.6, 0.03])
    slider = Slider(ax_slider, 'Point Size', 1, 200, valinit=initial_size, valfmt='%d')

    # 定义滑动条回调函数
    def update_multiview_size(val):
        """更新所有视图中点的大小"""
        new_size = slider.val

        # 更新所有散点图的大小
        scatter1.set_sizes([new_size] * len(x_coords))
        scatter2.set_sizes([new_size] * len(x_coords))
        scatter3.set_sizes([new_size] * len(x_coords))

        # 重新绘制
        fig.canvas.draw()

    # 连接滑动条和回调函数
    slider.on_changed(update_multiview_size)

    return fig, slider

def main():
    """Main function"""
    filename = 'coordinate.txt'

    print("Starting to read coordinate file...")
    x_coords, y_coords, z_coords = read_coordinates(filename)

    if x_coords is None:
        print("Program terminated")
        return

    print("\nCreating interactive 3D visualization...")

    # 创建带滑动条的3D可视化
    fig1, ax1, slider1 = visualize_3d_points_with_slider(x_coords, y_coords, z_coords)

    # 创建带滑动条的多视角显示
    print("\nCreating interactive multi-view display...")
    fig2, slider2 = create_multiple_views_with_sliders(x_coords, y_coords, z_coords)

    # 显示图形
    plt.show()

    # 可选：保存图片
    save_option = input("\nDo you want to save the images? (y/n): ").lower()
    if save_option == 'y':
        fig1.savefig('3d_coordinates_interactive.png', dpi=300, bbox_inches='tight')
        fig2.savefig('3d_coordinates_multiview_interactive.png', dpi=300, bbox_inches='tight')
        print("Interactive images saved as '3d_coordinates_interactive.png' and '3d_coordinates_multiview_interactive.png'")

if __name__ == "__main__":
    main()